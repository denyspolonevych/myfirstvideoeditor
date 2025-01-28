#include "XImagePro.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;

// Метод Set встановлює два зображення для подальшої обробки
void XImagePro::Set(cv::Mat mat1, cv::Mat mat2)
{
    if (mat1.empty()) return;  // Якщо перше зображення порожнє, виходимо з методу
    this->src1 = mat1;  // Збереження першого зображення в src1
    this->src2 = mat2;  // Збереження другого зображення в src2
    this->src1.copyTo(des);  // Копіюємо перше зображення в des для подальшої обробки
}

// Поворот зображення на 90 градусів за годинниковою стрілкою
void XImagePro::Rotate90()
{
    if (des.empty()) return;  // Якщо зображення не було встановлено, виходимо з методу
    rotate(des, des, ROTATE_90_CLOCKWISE);  // Поворот на 90 градусів за годинниковою стрілкою
}

// Поворот зображення на 180 градусів
void XImagePro::Rotate180()
{
    if (des.empty()) return;
    rotate(des, des, ROTATE_180);  // Поворот на 180 градусів
}

// Поворот зображення на 270 градусів (або 90 градусів проти годинникової стрілки)
void XImagePro::Rotate270()
{
    if (des.empty()) return;
    rotate(des, des, ROTATE_90_COUNTERCLOCKWISE);  // Поворот на 270 градусів
}

// Відбиття зображення по осі X (горизонтальне відображення)
void XImagePro::FlipX()
{
    if (des.empty()) return;
    flip(des, des, 0);  // Горизонтальне відображення
}

// Відбиття зображення по осі Y (вертикальне відображення)
void XImagePro::FlipY()
{
    if (des.empty()) return;
    flip(des, des, 1);  // Вертикальне відображення
}

// Відбиття зображення по обох осях (горизонтально і вертикально)
void XImagePro::FlipXY()
{
    if (des.empty()) return;
    flip(des, des, -1);  // Відбиття по обох осях
}

// Об'єднання двох зображень (side-by-side)
void XImagePro::Merge()
{
    if (des.empty()) return;
    if (src2.empty()) return;
    if (src2.size() != des.size())  // Якщо розміри зображень не співпадають
    {
        // Зміна розміру src2 так, щоб його висота відповідала висоті des
        int w = src2.cols * ((double)src2.rows / (double)des.rows);
        resize(src2, src2, Size(w, des.rows));
    }

    int dw = des.cols + src2.cols;  // Ширина об'єднаного зображення
    int dh = des.rows;  // Висота об'єднаного зображення
    des = Mat(Size(dw, dh), src1.type());  // Створюємо нове зображення для результату
    Mat r1 = des(Rect(0, 0, src1.cols, dh));  // Регіон для першого зображення
    Mat r2 = des(Rect(src1.cols, 0, src2.cols, dh));  // Регіон для другого зображення

    src1.copyTo(r1);  // Копіюємо перше зображення
    src2.copyTo(r2);  // Копіюємо друге зображення
}

// Змішування двох зображень за допомогою коефіцієнта alpha
void XImagePro::Blend(double a)
{
    if (des.empty()) return;
    if (src2.empty()) return;
    if (src2.size() != des.size())  // Якщо розміри зображень не співпадають
    {
        resize(src2, src2, des.size());  // Зміна розміру другого зображення
    }
    addWeighted(src2, a, des, 1 - a, 0, des);  // Змішування зображень
}

// Додавання мітки на зображення з використанням коефіцієнта alpha
void XImagePro::Mark(int x, int y, double a)
{
    if (des.empty()) return;
    if (src2.empty()) return;
    Mat rol = des(Rect(x, y, src2.cols, src2.rows));  // Вибір області для накладення
    addWeighted(src2, a, rol, 1 - a, 0, rol);  // Накладання зображення з міткою
}

// Зміна розміру зображення
void XImagePro::Resize(int width, int height)
{
    if (des.empty()) return;
    resize(des, des, Size(width, height));  // Зміна розміру зображення
}

// Зменшення розміру зображення за допомогою пірамідального зменшення
void XImagePro::PyDown(int count)
{
    if (des.empty()) return;
    for (int i = 0; i < count; i++)
        pyrDown(des, des);  // Зменшення зображення на один рівень
}

// Перетворення зображення в чорно-біле (градації сірого)
void XImagePro::Gray()
{
    if (des.empty()) return;
    cvtColor(des, des, COLOR_BGR2GRAY);  // Перетворення у відтінки сірого
}

// Обрізка зображення на зазначеній області
void XImagePro::Clip(int x, int y, int w, int h)
{
    if (des.empty()) return;
    if (x < 0 || y < 0 || w <= 0 || h <= 0) return;  // Перевірка на валідність координат
    if (x > des.cols || y > des.rows) return;  // Перевірка на виведення за межі зображення
    des = des(Rect(x, y, w, h));  // Обрізаємо зображення
}

// Збільшення розміру зображення за допомогою пірамідального збільшення
void XImagePro::PyUp(int count)
{
    if (des.empty()) return;
    for (int i = 0; i < count; i++)
        pyrUp(des, des);  // Збільшення зображення на один рівень
}

// Регулювання яскравості та контрасту зображення
void XImagePro::Gain(double bright, double contrast)
{
    if (des.empty()) return;
    des.convertTo(des, -1, contrast, bright);  // Застосування яскравості та контрасту
}

// Конструктор класу XImagePro
XImagePro::XImagePro()
{

}

// Деструктор класу XImagePro
XImagePro::~XImagePro()
{

}