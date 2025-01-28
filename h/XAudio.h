#pragma once
#include <string>

class XAudio
{
public:
    // Метод для отримання єдиного екземпляра класу XAudio (патерн Singleton)
    static XAudio* Get();

    // Експортує аудіо файл
    // src - шлях до вхідного аудіофайлу
    // out - шлях до вихідного аудіофайлу
    // beginMs - початковий час в мілісекундах для обрізки (за замовчуванням 0)
    // outMs - кінцевий час в мілісекундах для обрізки (за замовчуванням 0)
    virtual bool ExportA(std::string src, std::string out, int beginMs = 0, int outMs = 0) = 0;

    // Об'єднує відеофайл з аудіофайлом
    // v - шлях до відеофайлу
    // a - шлях до аудіофайлу
    // out - шлях до вихідного файлу, що містить об'єднані відео та аудіо
    virtual bool Merge(std::string v, std::string a, std::string out) = 0;

    // Конструктор класу XAudio
    XAudio();

    // Віртуальний деструктор класу XAudio
    virtual ~XAudio();
};