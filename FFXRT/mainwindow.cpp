#include <QTimer>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(updateAffection()));
    timer->start(1000);

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
    delete ui;
    timer->stop();
    delete timer;
    delete yuna;
    delete yunaH;
    delete auron;
    delete auronH;
    delete kimahri;
    delete kimahriH;
    delete wakka;
    delete wakkaH;
    delete lulu;
    delete luluH;
    delete rikku;
    delete rikkuH;
}

void MainWindow::clearText() {
    ui->yunaText->setText("");
    ui->auronText->setText("");
    ui->kimahriText->setText("");
    ui->wakkaText->setText("");
    ui->luluText->setText("");
    ui->rikkuText->setText("");
}

void MainWindow::readMemoryAndSetText(HANDLE handle, QLabel *label, DWORD address) {
    uint value = 0;
    ReadProcessMemory(handle,(void*)address,&value,sizeof(value),0);
    label->setText(QString::number(value));
    if(value > highestValue) {
        highestValue = value;
        resetPixmap();
        updatePixmap(label, true);
    } else if (value == highestValue) {
        updatePixmap(label, true);
    } else {
        updatePixmap(label, false);
    }
}

void MainWindow::updateAffection() {
    // try to find the FFX process
    DWORD pid;
    HWND ffx = FindWindow(FFX_CLASS_NAME, FFX_WINDOW_TITLE);
    if (ffx) {
        GetWindowThreadProcessId(ffx, &pid);
        HANDLE ffxHandle = OpenProcess(PROCESS_VM_READ, false, pid);
        if (ffxHandle) {
            readMemoryAndSetText(ffxHandle, ui->yunaText,    YUNA_ADDRESS);
            readMemoryAndSetText(ffxHandle, ui->auronText,   AURON_ADDRESS);
            readMemoryAndSetText(ffxHandle, ui->kimahriText, KIMAHRI_ADDRESS);
            readMemoryAndSetText(ffxHandle, ui->wakkaText,   WAKKA_ADDRESS);
            readMemoryAndSetText(ffxHandle, ui->luluText,    LULU_ADDRESS);
            readMemoryAndSetText(ffxHandle, ui->rikkuText,   RIKKU_ADDRESS);
        } else {
            clearText();
        }
    } else {
        clearText();
    }
}

void MainWindow::resetPixmap() {
    updatePixmap(ui->yunaText, false);
    updatePixmap(ui->auronText, false);
    updatePixmap(ui->kimahriText, false);
    updatePixmap(ui->wakkaText, false);
    updatePixmap(ui->luluText, false);
    updatePixmap(ui->rikkuText, false);
}

void MainWindow::updatePixmap(QLabel *label, boolean heart) {

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
