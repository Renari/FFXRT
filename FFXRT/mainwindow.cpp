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
    yunaText = findChild<QLabel *>("yunaText");
    auronText = findChild<QLabel *>("auronText");
    kimariText = findChild<QLabel *>("kimariText");
    wakkaText = findChild<QLabel *>("wakkaText");
    luluText = findChild<QLabel *>("luluText");
    rikkuText = findChild<QLabel *>("rikkuText");
    updateAffection();
}

MainWindow::~MainWindow()
{
    delete ui;
    timer->stop();
    delete timer;
    delete yunaText;
    delete auronText;
    delete kimariText;
    delete wakkaText;
    delete luluText;
    delete rikkuText;
}

void MainWindow::clearText()
{
    yunaText->setText("");
    auronText->setText("");
    kimariText->setText("");
    wakkaText->setText("");
    luluText->setText("");
    rikkuText->setText("");
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
            readMemoryAndSetText(ffxHandle, yunaText, YUNA_ADDRESS);
            readMemoryAndSetText(ffxHandle, auronText, AURON_ADDRESS);
            readMemoryAndSetText(ffxHandle, kimariText, KIMARI_ADDRESS);
            readMemoryAndSetText(ffxHandle, wakkaText, WAKKA_ADDRESS);
            readMemoryAndSetText(ffxHandle, luluText, LULU_ADDRESS);
            readMemoryAndSetText(ffxHandle, rikkuText, RIKKU_ADDRESS);
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
