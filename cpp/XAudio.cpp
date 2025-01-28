#include "XAudio.h"
#include <iostream>
#include <QMutex>

using namespace std;

// Структура для представлення часу у форматі години:хвилини:секунди.мілісекунди
struct XTime
{
    // Конструктор, що приймає час у мілісекундах і розбиває його на години, хвилини, секунди та мілісекунди
    XTime(int tms)
    {
        h = (tms / 1000) / 3600;  // Визначення годин
        m = ((tms / 1000) % 3600) / 60;  // Визначення хвилин
        s = (tms / 1000) % 60;  // Визначення секунд
        ms = tms % 1000;  // Визначення мілісекунд
    }

    // Перетворює час у рядковий формат
    std::string ToString()
    {
        char buf[16] = { 0 };
        sprintf(buf, "%d:%d:%d.%d", h, m, s, ms);
        return buf;
    }

    int h = 0;  // Години
    int m = 0;  // Хвилини
    int s = 0;  // Секунди
    int ms = 0; // Мілісекунди
};

// Клас CXAudio, що наслідує клас XAudio і реалізує його методи
class CXAudio : public XAudio
{
public:
    QMutex mutex; // М'ютекс для синхронізації доступу до команд

    // Реалізація методу для експорту аудіо з відеофайлу
    bool ExportA(std::string src, std::string out, int beginMs, int outMs)
    {
        // Формуємо команду для виконання через ffmpeg
        string cmd = "ffmpeg ";
        cmd += " -i ";
        cmd += src;
        cmd += " ";
        if (beginMs > 0)
        {
            cmd += " -ss ";
            XTime xt(beginMs);
            cmd += xt.ToString();
        }
        if (outMs > 0)
        {
            cmd += " -t ";
            XTime xt(outMs);
            cmd += xt.ToString();
        }
        cmd += " -vn -y ";
        cmd += out;
        cout << cmd << endl;

        // Виконуємо команду з блокуванням м'ютекса
        mutex.lock();
        system(cmd.c_str());
        mutex.unlock();
        return true;
    }

    // Реалізація методу для злиття аудіо та відео
    bool Merge(std::string v, std::string a, std::string out)
    {
        // Формуємо команду для виконання через ffmpeg
        string cmd = "ffmpeg -i ";
        cmd += v;
        cmd += " -i ";
        cmd += a;
        cmd += " -c copy ";
        cmd += out;
        cout << cmd << endl;

        // Виконуємо команду з блокуванням м'ютекса
        mutex.lock();
        system(cmd.c_str());
        mutex.unlock();
        return true;
    }
};

// Реалізація методу для отримання єдиного екземпляра класу XAudio (патерн Singleton)
XAudio* XAudio::Get()
{
    static CXAudio ca;
    return &ca;
}

// Конструктор класу XAudio
XAudio::XAudio()
{

}

// Деструктор класу XAudio
XAudio::~XAudio()
{

}