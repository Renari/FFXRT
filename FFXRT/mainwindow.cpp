#include <QTimer>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(updateAffection()));
    timer->start(1000);
    updateAffection();
}

MainWindow::~MainWindow()
{
    delete ui;
    timer->stop();
    delete timer;
}

void MainWindow::clearText()
{
    ui->yunaText->setText("");
    ui->auronText->setText("");
    ui->kimariText->setText("");
    ui->wakkaText->setText("");
    ui->luluText->setText("");
    ui->rikkuText->setText("");
}

void MainWindow::readMemoryAndSetText(HANDLE handle, QLabel *label, DWORD address)
{
    int value = 0;
    ReadProcessMemory(handle,(void*)address,&value,sizeof(value),0);
    label->setText(QString::number(value));
}

void MainWindow::updateAffection()
{
    // try to find the FFX process
    DWORD pid;
    HWND ffx = FindWindow(FFX_CLASS_NAME, FFX_WINDOW_TITLE);
    if (ffx)
    {
        GetWindowThreadProcessId(ffx, &pid);
        HANDLE ffxHandle = OpenProcess(PROCESS_VM_READ, false, pid);
        if (ffxHandle)
        {
            readMemoryAndSetText(ffxHandle, ui->yunaText, YUNA_ADDRESS);
            readMemoryAndSetText(ffxHandle, ui->auronText, AURON_ADDRESS);
            readMemoryAndSetText(ffxHandle, ui->kimariText, KIMARI_ADDRESS);
            readMemoryAndSetText(ffxHandle, ui->wakkaText, WAKKA_ADDRESS);
            readMemoryAndSetText(ffxHandle, ui->luluText, LULU_ADDRESS);
            readMemoryAndSetText(ffxHandle, ui->rikkuText, RIKKU_ADDRESS);
        }
        else
        {
            clearText();
        }
    }
    else
    {
        clearText();
    }
}
