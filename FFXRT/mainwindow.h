#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <array>
#include <QMainWindow>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
typedef unsigned long DWORD;
typedef void* HANDLE;
#endif

namespace Ui {
class MainWindow;
}

struct CharacterUI {
    QLabel   *textLabel;
    QLabel   *imageLabel;
    QPixmap   normalPixmap;
    QPixmap   heartPixmap;
    DWORD     addressOffset;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    unsigned int highestAffection = 0;
    std::array<CharacterUI, 6> characters;
    const QString FFX_PROCESS_NAME = "FFX.exe";
    const QString FFX_CLASS_NAME   = "PhyreFrameworkClass";
    const QString FFX_WINDOW_TITLE = "FINAL FANTASY X";
    void clearText();
    DWORD getBaseAddress(DWORD);
    void readMemoryAndSetText(HANDLE, CharacterUI&, DWORD);
private slots:
    void updateAffection();
};

#endif // MAINWINDOW_H
