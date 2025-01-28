#include "XVideoThread.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "XFilter.h"

using namespace std;
using namespace cv;

// Статичні змінні для зберігання відеофайлів та контролю їх обробки
static VideoCapture cap1;
static VideoCapture cap2;
static VideoWriter vw;
static bool isexit = false;

// Метод для початку збереження відео в файл
bool XVideoThread::StartSave(const std::string filename, int width, int height, bool isColor)
{
    cout << "Start export" << endl;
    Seek(begin);  // Перехід до початкового кадру
    mutex.lock();

    // Перевірка, чи відкрито відео джерело
    if (!cap1.isOpened())
    {
        mutex.unlock();
        return false;
    }

    // Якщо розміри відео не задані, беремо їх з першого відеофайлу
    if (width <= 0)
        width = cap1.get(CAP_PROP_FRAME_WIDTH);
    if (height <= 0)
        height = cap1.get(CAP_PROP_FRAME_HEIGHT);

    // Ініціалізація VideoWriter для збереження відео
    vw.open(filename,
        VideoWriter::fourcc('X', '2', '6', '4'),
        this->fps,
        Size(width, height), isColor
    );

    if (!vw.isOpened())
    {
        mutex.unlock();
        cout << "start save failed!" << endl;
        return false;
    }

    // Встановлення прапорця для збереження та запам'ятовування імені файлу
    this->isWrite = true;
    desFile = filename;
    mutex.unlock();
    return true;
}

// Метод для зупинки збереження відео
void XVideoThread::StopSave()
{
    cout << "stop export" << endl;
    mutex.lock();
    vw.release();  // Вивільнення ресурсу VideoWriter
    isWrite = false;
    mutex.unlock();
}

// Метод для встановлення початкового кадру для відтворення
void XVideoThread::SetBegin(double p)
{
    mutex.lock();
    double count = cap1.get(CAP_PROP_FRAME_COUNT);
    int frame = p * count;
    begin = frame;
    mutex.unlock();
}

// Метод для встановлення кінцевого кадру для відтворення
void XVideoThread::SetEnd(double p)
{
    mutex.lock();
    double count = cap1.get(CAP_PROP_FRAME_COUNT);
    int frame = p * count;
    end = frame;
    mutex.unlock();
}

// Метод для пошуку кадру по позиції
bool XVideoThread::Seek(double pos)
{
    double count = cap1.get(CAP_PROP_FRAME_COUNT);
    int frame = pos * count;
    return Seek(frame);
}

// Метод для перемотування відео до зазначеного кадру
bool XVideoThread::Seek(int frame)
{
    mutex.lock();
    if (!cap1.isOpened())
    {
        mutex.unlock();
        return false;
    }

    int re = cap1.set(CAP_PROP_POS_FRAMES, frame);  // Встановлення кадру для першого відео
    if (cap2.isOpened())
        cap2.set(CAP_PROP_POS_FRAMES, frame);  // Встановлення кадру для другого відео
    mutex.unlock();
    return re;
}

// Метод для отримання поточної позиції відео
double XVideoThread::GetPos()
{
    double pos = 0;
    mutex.lock();
    if (!cap1.isOpened())
    {
        mutex.unlock();
        return pos;
    }
    double count = cap1.get(CAP_PROP_FRAME_COUNT);
    double cur = cap1.get(CAP_PROP_POS_FRAMES);
    if (count > 0.001)
        pos = cur / count;  // Обчислення відносної позиції
    mutex.unlock();
    return pos;
}

// Метод для відкриття другого відеофайлу
bool XVideoThread::Open2(const std::string file)
{
    Seek(0);
    cout << "open2 :" << file << endl;
    mutex.lock();
    bool re = cap2.open(file);  // Відкриття другого відео
    mutex.unlock();
    cout << re << endl;
    if (!re)
        return re;

    // Зчитування розмірів другого відео
    width2 = cap2.get(CAP_PROP_FRAME_WIDTH);
    height2 = cap2.get(CAP_PROP_FRAME_HEIGHT);
    return true;
}

// Метод для відкриття першого відеофайлу
bool XVideoThread::Open(const std::string file)
{
    cout << "open :" << file << endl;
    Seek(0);
    mutex.lock();
    bool re = cap1.open(file);  // Відкриття першого відео
    mutex.unlock();

    cout << re << endl;
    if (!re)
        return re;

    fps = cap1.get(CAP_PROP_FPS);  // Отримання FPS з відео
    width = cap1.get(CAP_PROP_FRAME_WIDTH);  // Ширина відео
    height = cap1.get(CAP_PROP_FRAME_HEIGHT);  // Висота відео
    if (fps <= 0) fps = 25;  // Якщо FPS відсутній, встановлюємо за замовчуванням 25
    src1file = file;
    double count = cap1.get(CAP_PROP_FRAME_COUNT);  // Загальна кількість кадрів
    totalMs = (count / (double)fps) * 1000;  // Обчислення загальної тривалості відео в мілісекундах
    return true;
}

// Основний цикл потоку для обробки та відтворення/збереження відео
void XVideoThread::run()
{
    Mat mat1;
    while (!isexit)
    {
        mutex.lock();
        if (isexit)
        {
            mutex.unlock();
            break;
        }
        if (!cap1.isOpened())  // Якщо відео не відкрите
        {
            mutex.unlock();
            msleep(5);  // Чекаємо перед наступною спробою
            continue;
        }
        if (!isPlay)  // Якщо відтворення не активне
        {
            mutex.unlock();
            msleep(5);
            continue;
        }

        int cur = cap1.get(CAP_PROP_POS_FRAMES);  // Поточний кадр
        if ((end > 0 && cur >= end) || !cap1.read(mat1) || mat1.empty())  // Якщо досягнуто кінця відео або не вдалось прочитати кадр
        {
            mutex.unlock();
            if (isWrite)
            {
                StopSave();  // Зупинка запису
                SaveEnd();  // Виклик завершення збереження
            }

            msleep(5);
            continue;
        }

        Mat mat2 = mark;
        if (cap2.isOpened())  // Якщо друге відео відкрито
        {
            cap2.read(mat2);  // Читання кадру з другого відео
        }

        if (!isWrite)
        {
            ViewImage1(mat1);  // Перегляд першого зображення
            if (!mat2.empty())
                ViewImage2(mat2);  // Перегляд другого зображення
        }

        // Застосування фільтрів до кадрів
        Mat des = XFilter::Get()->Filter(mat1, mat2);

        if (!isWrite)
            ViewDes(des);  // Перегляд результату обробки

        int s = 1000 / fps;  // Затримка між кадрами
        if (isWrite)
        {
            s = 1;  // Для запису використовуємо мінімальну затримку
            vw.write(des);  // Запис результату в файл
        }

        msleep(s);  // Часова затримка між кадрами
        mutex.unlock();
    }
}

// Конструктор для ініціалізації потоку
XVideoThread::XVideoThread()
{
    start();  // Запуск потоку
}

// Деструктор для коректного завершення потоку
XVideoThread::~XVideoThread()
{
    mutex.lock();
    isexit = true;  // Встановлення прапорця для завершення потоку
    mutex.unlock();
    wait();  // Очікування завершення потоку
}