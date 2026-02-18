#ifdef _WIN32
#include <Windows.h>
#include <TlHelp32.h>
#else
#include <sys/uio.h>
#include <dirent.h>
#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#endif
#include <QTimer>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateAffection);
    timer->start(1000);

    characters = {{
        { ui->yunaText,    ui->yunaImage,    QPixmap(":/images/Yuna.png"),    QPixmap(":/images/YunaH.png"),    0xD2CAC0 },
        { ui->auronText,   ui->auronImage,   QPixmap(":/images/Auron.png"),   QPixmap(":/images/AuronH.png"),   0xD2CAC4 },
        { ui->kimahriText, ui->kimahriImage, QPixmap(":/images/Kimahri.png"), QPixmap(":/images/KimahriH.png"), 0xD2CAC8 },
        { ui->wakkaText,   ui->wakkaImage,   QPixmap(":/images/Wakka.png"),   QPixmap(":/images/WakkaH.png"),   0xD2CACC },
        { ui->luluText,    ui->luluImage,    QPixmap(":/images/Lulu.png"),    QPixmap(":/images/LuluH.png"),    0xD2CAD0 },
        { ui->rikkuText,   ui->rikkuImage,   QPixmap(":/images/Rikku.png"),   QPixmap(":/images/RikkuH.png"),   0xD2CAD4 },
    }};

    QString textStyle = "QLabel { color: black; }";
    for (auto& c : characters) {
        c.textLabel->setStyleSheet(textStyle);
    }

    updateAffection();
}

MainWindow::~MainWindow() {
    timer->stop();
    delete ui;
}

void MainWindow::clearText() {
    for (auto& c : characters) {
        c.textLabel->setText("");
        c.imageLabel->setPixmap(c.normalPixmap);
    }
}

DWORD MainWindow::getBaseAddress(DWORD pid)
{
#ifdef _WIN32
    MODULEENTRY32 module;
    module.dwSize = sizeof(MODULEENTRY32);
    HANDLE moduleHandle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (moduleHandle) {
        if(Module32First(moduleHandle, &module)) {
            do {
                if(!wcscmp(module.szModule, (const wchar_t*)FFX_PROCESS_NAME.utf16())) {
                    return (unsigned int)module.modBaseAddr;
                }
            } while(Module32Next(moduleHandle, &module));
        }
    }
    return 0;
#else
    std::string mapsPath = "/proc/" + std::to_string(pid) + "/maps";
    std::ifstream mapsFile(mapsPath);
    std::string line;
    if (mapsFile.is_open()) {
        while (std::getline(mapsFile, line)) {
            if (line.find(FFX_PROCESS_NAME.toStdString()) != std::string::npos) {
                size_t dashPos = line.find('-');
                if (dashPos != std::string::npos) {
                    std::string addrStr = line.substr(0, dashPos);
                    return std::stoul(addrStr, nullptr, 16);
                }
            }
        }
    }
    return 0;
#endif
}

void MainWindow::readMemoryAndSetText(HANDLE handle, CharacterUI& character, DWORD baseAddress) {
    unsigned int value = 0;
    DWORD address = baseAddress + character.addressOffset;
#ifdef _WIN32
    if (!ReadProcessMemory(handle, (void*)address, &value, sizeof(value), 0)) {
        return;
    }
#else
    struct iovec local[1];
    struct iovec remote[1];
    local[0].iov_base = &value;
    local[0].iov_len = sizeof(value);
    remote[0].iov_base = (void*)(uintptr_t)address;
    remote[0].iov_len = sizeof(value);

    // In Linux implementation, 'handle' is just the PID cast to void*
    pid_t pid = (pid_t)(intptr_t)handle;
    if (process_vm_readv(pid, local, 1, remote, 1, 0) < 0) {
        return;
    }
#endif
    character.textLabel->setText(QString::number(value));
    if (value > highestAffection) {
        highestAffection = value;
        // Reset all to normal, then highlight this one
        for (auto& c : characters) {
            c.imageLabel->setPixmap(c.normalPixmap);
        }
        character.imageLabel->setPixmap(character.heartPixmap);
    } else if (highestAffection != 0 && value == highestAffection) {
        character.imageLabel->setPixmap(character.heartPixmap);
    } else {
        character.imageLabel->setPixmap(character.normalPixmap);
    }
}

void MainWindow::updateAffection() {
    // clear highest value since we're updating
    highestAffection = 0;
    DWORD pid = 0;

#ifdef _WIN32
    // try to find the FFX process
    HWND ffx = FindWindow((const wchar_t*)FFX_CLASS_NAME.utf16(), (const wchar_t*)FFX_WINDOW_TITLE.utf16());
    if (ffx) {
        GetWindowThreadProcessId(ffx, &pid);
        DWORD baseAddress = getBaseAddress(pid);
        if (baseAddress) {
            HANDLE ffxHandle = OpenProcess(PROCESS_VM_READ, false, pid);
            if (ffxHandle) {
                for (auto& c : characters) {
                    readMemoryAndSetText(ffxHandle, c, baseAddress);
                }
                return;
            }
        }
    }
#else
    // Linux implementation: iterate /proc to find "FFX.exe"
    DIR* procDir = opendir("/proc");
    if (procDir) {
        struct dirent* entry;
        while ((entry = readdir(procDir)) != nullptr) {
            if (entry->d_type == DT_DIR) {
                std::string pidStr = entry->d_name;
                if (std::all_of(pidStr.begin(), pidStr.end(), ::isdigit)) {
                    std::string cmdlinePath = "/proc/" + pidStr + "/cmdline";
                    std::ifstream cmdlineFile(cmdlinePath);
                    std::string cmdline;
                    if (cmdlineFile.is_open()) {
                        std::getline(cmdlineFile, cmdline, '\0');
                        if (cmdline.find(FFX_PROCESS_NAME.toStdString()) != std::string::npos) {
                            pid = (DWORD)std::stoul(pidStr);
                            break;
                        }
                    }
                }
            }
        }
        closedir(procDir);
    }

    if (pid != 0) {
        DWORD baseAddress = getBaseAddress(pid);
        if (baseAddress) {
            // Use PID as handle
            HANDLE ffxHandle = (HANDLE)(intptr_t)pid;
            for (auto& c : characters) {
                readMemoryAndSetText(ffxHandle, c, baseAddress);
            }
            return;
        }
    }
#endif
    clearText();
}
