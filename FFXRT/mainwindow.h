#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <array>
#include <cstdint>
#include <QMainWindow>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#endif

namespace Ui {
class MainWindow;
}

struct CharacterUI {
    QLabel   *textLabel;
    QLabel   *imageLabel;
    QPixmap   normalPixmap;
    QPixmap   heartPixmap;
    uint32_t  addressOffset;
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
#ifdef _WIN32
    DWORD getBaseAddress(DWORD pid);
    void readMemoryAndSetText(HANDLE handle, CharacterUI& character, DWORD baseAddress);
#else
    uint32_t getBaseAddress(pid_t pid);
    void readMemoryAndSetText(pid_t pid, CharacterUI& character, uint32_t baseAddress);
#endif
private slots:
    void updateAffection();
};

#endif // MAINWINDOW_H
