#include "XVideoUI.h"
#include <QtWidgets/QApplication>
//#include "XAudio.h"

int main(int argc, char* argv[])
{
    // Експортує аудіо з відеофайлу "test3.mp4" у файл "test3.mp3"
    //XAudio::Get()->ExportA("test3.mp4", "test3.mp3");

    // Об'єднує відеофайл "test3.avi" з аудіофайлом "test3.mp3" і зберігає у файл "out3.avi"
    //XAudio::Get()->Merge("test3.avi", "test3.mp3", "out3.avi");

    // Створює екземпляр застосунку Qt
    QApplication a(argc, argv);

    // Створює екземпляр головного вікна відеоредактора
    XVideoUI w;

    // Відображає головне вікно
    w.show();

    // Встановлює заголовок головного вікна
    w.setWindowTitle("Полоневич Денис. Відеоредактор (Дипломна робота)");

    // Запускає головний цикл застосунку
    return a.exec();
}