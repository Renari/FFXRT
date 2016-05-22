#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <Windows.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    unsigned int highestAffection = 0;
    const DWORD YUNA_ADDRESS = 0xD2CAC0;
    const DWORD AURON_ADDRESS = 0xD2CAC4;
    const DWORD KIMAHRI_ADDRESS = 0xD2CAC8;
    const DWORD WAKKA_ADDRESS = 0xD2CACC;
    const DWORD LULU_ADDRESS = 0xD2CAD0;
    const DWORD RIKKU_ADDRESS = 0xD2CAD4;
    const LPCWSTR FFX_PROCESS_NAME = L"FFX.exe";
    const LPCWSTR FFX_CLASS_NAME = L"PhyreFrameworkClass";
    const LPCWSTR FFX_WINDOW_TITLE = L"FINAL FANTASY X";
    QPixmap yuna;
    QPixmap yunaH;
    QPixmap auron;
    QPixmap auronH;
    QPixmap kimahri;
    QPixmap kimahriH;
    QPixmap wakka;
    QPixmap wakkaH;
    QPixmap lulu;
    QPixmap luluH;
    QPixmap rikku;
    QPixmap rikkuH;
    void clearText();
    DWORD getBaseAddress(DWORD);
    void readMemoryAndSetText(HANDLE, QLabel*, DWORD);
    void resetPixmap();
    void updatePixmap(QLabel*, boolean);
private slots:
    void updateAffection();
};

#endif // MAINWINDOW_H
