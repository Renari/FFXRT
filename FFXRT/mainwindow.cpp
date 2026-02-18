#ifdef _WIN32
#include <Windows.h>
#include <TlHelp32.h>
#else
#include <sys/uio.h>
#include <dirent.h>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <cstdlib>
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

    QString textStyle = "QLabel { color: black; }";
    ui->yunaText->setStyleSheet(textStyle);
    ui->auronText->setStyleSheet(textStyle);
    ui->kimahriText->setStyleSheet(textStyle);
    ui->wakkaText->setStyleSheet(textStyle);
    ui->luluText->setStyleSheet(textStyle);
    ui->rikkuText->setStyleSheet(textStyle);

    yuna = QPixmap(":/images/Yuna.png");
    yunaH = QPixmap(":/images/YunaH.png");
    auron = QPixmap(":/images/Auron.png");
    auronH = QPixmap(":/images/AuronH.png");
    kimahri = QPixmap(":/images/Kimahri.png");
    kimahriH = QPixmap(":/images/KimahriH.png");
    wakka = QPixmap(":/images/Wakka.png");
    wakkaH = QPixmap(":/images/WakkaH.png");
    lulu = QPixmap(":/images/Lulu.png");
    luluH = QPixmap(":/images/LuluH.png");
    rikku = QPixmap(":/images/Rikku.png");
    rikkuH = QPixmap(":/images/RikkuH.png");

    updateAffection();
}

MainWindow::~MainWindow() {
    timer->stop();
    delete ui;
}

void MainWindow::clearText() {
    ui->yunaText->setText("");
    ui->auronText->setText("");
    ui->kimahriText->setText("");
    ui->wakkaText->setText("");
    ui->luluText->setText("");
    ui->rikkuText->setText("");
    resetPixmap();
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

void MainWindow::readMemoryAndSetText(HANDLE handle, QLabel *label, DWORD address) {
    unsigned int value = 0;
#ifdef _WIN32
    ReadProcessMemory(handle,(void*)address,&value,sizeof(value),0);
#else
    struct iovec local[1];
    struct iovec remote[1];
    local[0].iov_base = &value;
    local[0].iov_len = sizeof(value);
    remote[0].iov_base = (void*)(uintptr_t)address;
    remote[0].iov_len = sizeof(value);
    
    // In Linux implementation, 'handle' is just the PID cast to void*
    pid_t pid = (pid_t)(intptr_t)handle;
    process_vm_readv(pid, local, 1, remote, 1, 0);
#endif
    label->setText(QString::number(value));
    if(value > highestAffection) {
        highestAffection = value;
        resetPixmap();
        updatePixmap(label, true);
    } else if (highestAffection != 0 && value == highestAffection) {
        updatePixmap(label, true);
    } else {
        updatePixmap(label, false);
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
                readMemoryAndSetText(ffxHandle, ui->yunaText,    baseAddress+YUNA_ADDRESS);
                readMemoryAndSetText(ffxHandle, ui->auronText,   baseAddress+AURON_ADDRESS);
                readMemoryAndSetText(ffxHandle, ui->kimahriText, baseAddress+KIMAHRI_ADDRESS);
                readMemoryAndSetText(ffxHandle, ui->wakkaText,   baseAddress+WAKKA_ADDRESS);
                readMemoryAndSetText(ffxHandle, ui->luluText,    baseAddress+LULU_ADDRESS);
                readMemoryAndSetText(ffxHandle, ui->rikkuText,   baseAddress+RIKKU_ADDRESS);
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
            readMemoryAndSetText(ffxHandle, ui->yunaText,    baseAddress+YUNA_ADDRESS);
            readMemoryAndSetText(ffxHandle, ui->auronText,   baseAddress+AURON_ADDRESS);
            readMemoryAndSetText(ffxHandle, ui->kimahriText, baseAddress+KIMAHRI_ADDRESS);
            readMemoryAndSetText(ffxHandle, ui->wakkaText,   baseAddress+WAKKA_ADDRESS);
            readMemoryAndSetText(ffxHandle, ui->luluText,    baseAddress+LULU_ADDRESS);
            readMemoryAndSetText(ffxHandle, ui->rikkuText,   baseAddress+RIKKU_ADDRESS);
            return;
        }
    }
#endif
    clearText();
}

void MainWindow::resetPixmap() {
    updatePixmap(ui->yunaText,    false);
    updatePixmap(ui->auronText,   false);
    updatePixmap(ui->kimahriText, false);
    updatePixmap(ui->wakkaText,   false);
    updatePixmap(ui->luluText,    false);
    updatePixmap(ui->rikkuText,   false);
}

void MainWindow::updatePixmap(QLabel *label, bool heart) {

    if (label == ui->yunaText) {
        if (heart) {
            ui->yunaImage->setPixmap(yunaH);
        } else {
            ui->yunaImage->setPixmap(yuna);
        }

    }

    if (label == ui->auronText) {
        if (heart) {
            ui->auronImage->setPixmap(auronH);
        } else {
            ui->auronImage->setPixmap(auron);
        }
    }

    if (label == ui->kimahriText) {
        if (heart) {
            ui->kimahriImage->setPixmap(kimahriH);
        } else {
            ui->kimahriImage->setPixmap(kimahri);
        }
    }

    if (label == ui->wakkaText) {
        if (heart) {
            ui->wakkaImage->setPixmap(wakkaH);
        } else {
            ui->wakkaImage->setPixmap(wakka);
        }
    }

    if (label == ui->luluText) {
        if (heart) {
            ui->luluImage->setPixmap(luluH);
        } else {
            ui->luluImage->setPixmap(lulu);
        }
    }

    if (label == ui->rikkuText) {
        if (heart) {
            ui->rikkuImage->setPixmap(rikkuH);
        } else {
            ui->rikkuImage->setPixmap(rikku);
        }
    }
}
