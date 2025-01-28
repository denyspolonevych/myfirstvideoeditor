#include "xvideowidget.h" // Підключаємо заголовочний файл для віджету відео.
#include <QPainter> // Для малювання на віджеті.
#include <opencv2/imgproc.hpp> // Для обробки зображень за допомогою OpenCV.

using namespace cv;

// Функція для встановлення зображення в віджет
void XVideoWidget::SetImage(cv::Mat mat)
{
    QImage::Format fmt = QImage::Format_RGB888; // Формат зображення за замовчуванням (кольорове)
    int pixSize = 3; // Розмір пікселя (3 байти для кольорового зображення)

    // Якщо зображення чорно-біле, змінюємо формат та розмір пікселя
    if (mat.type() == CV_8UC1)
    {
        fmt = QImage::Format_Grayscale8; // Формат для чорно-білих зображень
        pixSize = 1; // Один байт на піксель
    }

    // Якщо зображення відсутнє або формат не відповідає поточному, створюємо нове
    if (img.isNull() || img.format() != fmt)
    {
        delete img.bits(); // Видаляємо старі дані зображення
        uchar* buf = new uchar[width() * height() * pixSize]; // Створюємо новий буфер для зображення
        img = QImage(buf, width(), height(), fmt); // Ініціалізуємо нове зображення
    }

    Mat des;

    // Масштабуємо зображення до розміру віджету
    cv::resize(mat, des, Size(img.size().width(), img.size().height()));

    // Якщо зображення кольорове, конвертуємо його з BGR в RGB
    if (pixSize > 1)
        cv::cvtColor(des, des, COLOR_BGR2RGB);

    // Копіюємо дані зображення в буфер
    memcpy(img.bits(), des.data, des.cols * des.rows * des.elemSize());

    update(); // Оновлюємо віджет для відображення нового зображення
}

// Конструктор класу XVideoWidget
XVideoWidget::XVideoWidget(QWidget* p) : QOpenGLWidget(p)
{
    // Пустий конструктор, викликає базовий конструктор QOpenGLWidget
}

// Обробка події малювання на віджеті
void XVideoWidget::paintEvent(QPaintEvent* e)
{
    QPainter p; // Створюємо об'єкт для малювання
    p.begin(this); // Починаємо малювання на поточному віджеті
    p.drawImage(QPoint(0, 0), img); // Малюємо зображення в точці (0,0)
    p.end(); // Завершуємо малювання
}

// Деструктор класу XVideoWidget
XVideoWidget::~XVideoWidget()
{
    // Деструктор не містить специфічної логіки, але при потребі можна додати очищення ресурсів
}