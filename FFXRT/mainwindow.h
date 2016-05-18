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
    const DWORD YUNA_ADDRESS = 0x01D0CAC0;
    const DWORD AURON_ADDRESS = 0x01D0CAC4;
    const DWORD KIMARI_ADDRESS = 0x01D0CAC8;
    const DWORD WAKKA_ADDRESS = 0x01D0CACC;
    const DWORD LULU_ADDRESS = 0x01D0CAD0;
    const DWORD RIKKU_ADDRESS = 0x01D0CAD4;
    const LPCWSTR FFX_CLASS_NAME = L"PhyreFrameworkClass";
    const LPCWSTR FFX_WINDOW_TITLE = L"Final Fantasy X";
    void readMemoryAndSetText(HANDLE, QLabel*, DWORD);
    void clearText();
private slots:
    void updateAffection();
};

#endif // MAINWINDOW_H
