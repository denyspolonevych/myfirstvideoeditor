#pragma once
#include <opencv2/core.hpp>
#include <vector>

// Перелічення типів завдань для обробки зображень
enum XTaskType
{
    XTASK_NONE,       // Немає завдання
    XTASK_GAIN,       // Налаштування яскравості та контрастності
    XTASK_ROTATE90,   // Поворот на 90 градусів
    XTASK_ROTATE180,  // Поворот на 180 градусів
    XTASK_ROTATE270,  // Поворот на 270 градусів
    XTASK_FLIPX,      // Відображення по горизонталі
    XTASK_FLIPY,      // Відображення по вертикалі
    XTASK_FLIPXY,     // Відображення по горизонталі та вертикалі
    XTASK_RESIZE,     // Зміна розміру
    XTASK_PYDOWN,     // Зменшення розміру за допомогою пірамідального зменшення
    XTASK_PYUP,       // Збільшення розміру за допомогою пірамідального збільшення
    XTASK_CLIP,       // Обрізка зображення
    XTASK_GRAY,       // Перетворення в градації сірого
    XTASK_MARK,       // Нанесення мітки
    XTASK_BLEND,      // Змішування зображень
    XTASK_MERGE       // Об'єднання зображень
};

// Структура, що представляє завдання для обробки зображень
struct XTask
{
    XTaskType type;            // Тип завдання
    std::vector<double> para;  // Параметри завдання
};

// Абстрактний клас для фільтра обробки зображень
class XFilter
{
public:
    // Фільтрує два вхідних зображення і повертає результуюче зображення
    // mat1 - перше вхідне зображення
    // mat2 - друге вхідне зображення
    // Повертає: результуюче зображення (cv::Mat)
    virtual cv::Mat Filter(cv::Mat mat1, cv::Mat mat2) = 0;

    // Додає завдання до черги обробки
    // task - завдання для обробки
    virtual void Add(XTask task) = 0;

    // Очищає чергу завдань
    virtual void Clear() = 0;

    // Метод для отримання єдиного екземпляра класу XFilter (патерн Singleton)
    static XFilter* Get();

    // Віртуальний деструктор класу XFilter
    virtual ~XFilter();

protected:
    // Захищений конструктор класу XFilter
    XFilter();
};