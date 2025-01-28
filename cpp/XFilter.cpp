#include "XFilter.h"
#include <QThread>
#include <QMutex>
#include "XImagePro.h"

// Клас CXFilter, що наслідує абстрактний клас XFilter і реалізує його методи
class CXFilter : public XFilter
{
public:
    std::vector<XTask> tasks;  // Список завдань для обробки
    QMutex mutex;  // М'ютекс для синхронізації доступу до списку завдань та обробки зображень

    // Метод для обробки зображень з використанням заданих завдань
    cv::Mat Filter(cv::Mat mat1, cv::Mat mat2)
    {
        mutex.lock();  // Блокування м'ютекса для синхронізації доступу до даних
        XImagePro p;  // Створення об'єкта для обробки зображень
        p.Set(mat1, mat2);  // Встановлення зображень для обробки

        // Проходження по всіх завданнях і виконання відповідних методів для кожного
        for (int i = 0; i < tasks.size(); i++)
        {
            switch (tasks[i].type)  // Вибір операції залежно від типу завдання
            {
            case XTASK_GAIN:
                p.Gain(tasks[i].para[0], tasks[i].para[1]);  // Корекція яскравості та контрасту
                break;
            case XTASK_ROTATE90:
                p.Rotate90();  // Поворот на 90 градусів
                break;
            case XTASK_ROTATE180:
                p.Rotate180();  // Поворот на 180 градусів
                break;
            case XTASK_ROTATE270:
                p.Rotate270();  // Поворот на 270 градусів
                break;

            case XTASK_FLIPX:
                p.FlipX();  // Відбиття по осі X
                break;
            case XTASK_FLIPY:
                p.FlipY();  // Відбиття по осі Y
                break;
            case XTASK_FLIPXY:
                p.FlipXY();  // Відбиття по обох осях X та Y
                break;
            case XTASK_RESIZE:
                p.Resize(tasks[i].para[0], tasks[i].para[1]);  // Зміна розміру зображення
                break;

            case XTASK_PYDOWN:
                p.PyDown(tasks[i].para[0]);  // Зменшення розміру за допомогою пірамідального зменшення
                break;
            case XTASK_PYUP:
                p.PyUp(tasks[i].para[0]);  // Збільшення розміру за допомогою пірамідального збільшення
                break;
            case XTASK_CLIP:
                p.Clip(tasks[i].para[0], tasks[i].para[1], tasks[i].para[2], tasks[i].para[3]);  // Обрізка зображення
                break;
            case XTASK_GRAY:
                p.Gray();  // Перетворення зображення в чорно-біле (градації сірого)
                break;
            case XTASK_MARK:
                p.Mark(tasks[i].para[0], tasks[i].para[1], tasks[i].para[2]);  // Додавання мітки на зображення
                break;
            case XTASK_BLEND:
                p.Blend(tasks[i].para[0]);  // Змішування зображень
                break;
            case XTASK_MERGE:
                p.Merge();  // Об'єднання зображень
                break;
            default:
                break;  // Якщо завдання не відомо, просто пропускаємо
            }
        }

        // Отримуємо результат після виконання всіх завдань
        cv::Mat re = p.Get();
        mutex.unlock();  // Відблокування м'ютекса
        return re;  // Повертаємо результат
    }

    // Метод для додавання завдання до списку завдань
    void Add(XTask task)
    {
        mutex.lock();  // Блокування м'ютекса для безпечного доступу до списку завдань
        tasks.push_back(task);  // Додавання нового завдання
        mutex.unlock();  // Відблокування м'ютекса
    }

    // Метод для очищення списку завдань
    void Clear()
    {
        mutex.lock();  // Блокування м'ютекса для безпечного доступу до списку
        tasks.clear();  // Очищаємо список завдань
        mutex.unlock();  // Відблокування м'ютекса
    }
};

// Реалізація методу Get() для отримання єдиного екземпляра класу XFilter (Singleton)
XFilter* XFilter::Get()
{
    static CXFilter cx;  // Створюємо єдиний екземпляр класу
    return &cx;  // Повертаємо його
}

// Конструктор класу XFilter
XFilter::XFilter()
{

}

// Деструктор класу XFilter
XFilter::~XFilter()
{

}