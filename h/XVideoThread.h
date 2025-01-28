#pragma once
#include <QThread>
#include <QMutex>
#include "opencv2/core.hpp"

class XVideoThread : public QThread
{
    Q_OBJECT
public:
    int fps = 0;         // Кількість кадрів за секунду
    int width = 0;       // Ширина відео
    int height = 0;      // Висота відео
    int width2 = 0;      // Ширина другого відео (якщо використовується)
    int height2 = 0;     // Висота другого відео (якщо використовується)
    int totalMs = 0;     // Загальна тривалість відео в мілісекундах
    std::string src1file; // Шлях до першого відеофайлу
    std::string desFile;  // Шлях до вихідного відеофайлу

    int begin = 0;       // Початковий кадр для обробки
    int end = 0;         // Кінцевий кадр для обробки

    // Встановлює початковий кадр для обробки
    // p - частка від загальної кількості кадрів (від 0 до 1)
    void SetBegin(double p);

    // Встановлює кінцевий кадр для обробки
    // p - частка від загальної кількості кадрів (від 0 до 1)
    void SetEnd(double p);

    // Метод для отримання єдиного екземпляра класу XVideoThread (патерн Singleton)
    static XVideoThread* Get()
    {
        static XVideoThread vt;
        return &vt;
    }

    // Відкриває відеофайл для обробки
    // file - шлях до відеофайлу
    // Повертає true, якщо файл успішно відкрито, і false, якщо ні
    bool Open(const std::string file);

    // Відкриває другий відеофайл для обробки
    // file - шлях до другого відеофайлу
    // Повертає true, якщо файл успішно відкрито, і false, якщо ні
    bool Open2(const std::string file);

    // Починає відтворення відео
    void Play() { mutex.lock(); isPlay = true; mutex.unlock(); }

    // Призупиняє відтворення відео
    void Pause() { mutex.lock(); isPlay = false; mutex.unlock(); }

    // Повертає поточну позицію відтворення у відсотках
    // Повертає: частка від загальної тривалості (від 0 до 1)
    double GetPos();

    // Переходить до певного кадру відео
    // frame - номер кадру
    // Повертає true, якщо перехід успішний, і false, якщо ні
    bool Seek(int frame);

    // Переходить до певної позиції у відео
    // pos - частка від загальної тривалості (від 0 до 1)
    // Повертає true, якщо перехід успішний, і false, якщо ні
    bool Seek(double pos);

    // Починає збереження відео у файл
    // filename - шлях до вихідного файлу
    // width - ширина відео (за замовчуванням 0, використовується ширина вхідного відео)
    // height - висота відео (за замовчуванням 0, використовується висота вхідного відео)
    // isColor - збереження кольорового відео (за замовчуванням true)
    // Повертає true, якщо збереження успішно розпочато, і false, якщо ні
    bool StartSave(const std::string filename, int width = 0, int height = 0, bool isColor = true);

    // Зупиняє збереження відео у файл
    void StopSave();

    // Встановлює мітку на відео
    // m - зображення мітки (cv::Mat)
    void SetMark(cv::Mat m) { mutex.lock(); this->mark = m; mutex.unlock(); }

    // Деструктор класу XVideoThread
    ~XVideoThread();

    // Основний метод виконання потоку
    void run();

signals:
    // Сигнал для відображення першого кадру відео
    void ViewImage1(cv::Mat mat);

    // Сигнал для відображення другого кадру відео
    void ViewImage2(cv::Mat mat);

    // Сигнал для відображення результуючого кадру
    void ViewDes(cv::Mat mat);

    // Сигнал про завершення збереження відео
    void SaveEnd();

protected:
    QMutex mutex;         // М'ютекс для синхронізації доступу до спільних ресурсів
    bool isWrite = false; // Прапорець для визначення, чи здійснюється запис
    bool isPlay = false;  // Прапорець для визначення, чи здійснюється відтворення
    cv::Mat mark;         // Мітка для накладення на відео

    // Захищений конструктор класу XVideoThread
    XVideoThread();
};