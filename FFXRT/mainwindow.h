#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
typedef unsigned long DWORD;
typedef void* HANDLE;
typedef unsigned short WORD;
typedef const char* LPCSTR;
typedef const wchar_t* LPCWSTR;
typedef void* HWND;
#endif

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
    const QString FFX_PROCESS_NAME = "FFX.exe";
    const QString FFX_CLASS_NAME = "PhyreFrameworkClass";
    const QString FFX_WINDOW_TITLE = "FINAL FANTASY X";
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
    void updatePixmap(QLabel*, bool);
private slots:
    void updateAffection();
};

#endif // MAINWINDOW_H
