#include "XVideoUI.h" // Підключаємо заголовочний файл для інтерфейсу відео.
#include <QFileDialog>  // Для відкриття діалогового вікна для вибору файлів.
#include <string> // Підключаємо стандартну бібліотеку для роботи з рядками.
#include <QMessageBox> // Для відображення повідомлень користувачеві.
#include "XVideoThread.h" // Підключаємо клас, який обробляє потоки відео.
#include "XFilter.h" // Підключаємо клас для фільтрації відео.
#include <opencv2/imgcodecs.hpp> // Для читання і запису зображень.
#include <opencv2/imgproc.hpp> // Для обробки зображень.
#include <opencv2/highgui.hpp> // Для роботи з вікнами для відображення зображень.
#include "XAudio.h" // Для роботи з аудіо.
#include <QFile> // Для роботи з файлами в Qt.

using namespace std;
using namespace cv;

// Статичні змінні, що зберігають стани інтерфейсу користувача.
static bool pressSlider = false; // Змінна, яка вказує, чи натиснуто слайдер.
static bool isExport = false; // Змінна, яка вказує, чи зараз виконується експорт відео.
static bool isColor = true; // Змінна, яка вказує, чи застосовувати кольорове зображення.
static bool isMark = false; // Змінна, яка вказує, чи потрібно накладати маркування.
static bool isBlend = false; // Змінна, яка вказує, чи потрібно застосовувати змішування зображень.
static bool isMerge = false; // Змінна, яка вказує, чи потрібно об'єднувати два відео.

// Конструктор інтерфейсу користувача XVideoUI
XVideoUI::XVideoUI(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this); // Налаштовуємо графічний інтерфейс
	//setWindowFlags(Qt::FramelessWindowHint); // Закоментовано, але можна використати для безрамкових вікон
	qRegisterMetaType<cv::Mat>("cv::Mat"); // Реєструємо тип cv::Mat для використання в сигналах Qt

	// Підключаємо сигнали та слоти для відображення зображень у віджетах
	QObject::connect(XVideoThread::Get(),
		SIGNAL(ViewImage1(cv::Mat)),
		ui.src1,
		SLOT(SetImage(cv::Mat))
	);
	QObject::connect(XVideoThread::Get(),
		SIGNAL(ViewImage2(cv::Mat)),
		ui.src2,
		SLOT(SetImage(cv::Mat))
	);

	QObject::connect(XVideoThread::Get(),
		SIGNAL(ViewDes(cv::Mat)),
		ui.des,
		SLOT(SetImage(cv::Mat))
	);

	// Підключення сигналу для завершення експорту
	QObject::connect(XVideoThread::Get(),
		SIGNAL(SaveEnd()),
		this,
		SLOT(ExportEnd())
	);
	Pause(); // Початкова зупинка відтворення
	startTimer(40); // Запускаємо таймер для оновлення інтерфейсу
}

// Обробка події таймера для оновлення слайдера
void XVideoUI::timerEvent(QTimerEvent* e)
{
	if (pressSlider) return; // Якщо слайдер натиснуто, оновлення не виконується
	double pos = XVideoThread::Get()->GetPos(); // Отримуємо поточну позицію відео
	ui.playSlider->setValue(pos * 1000); // Оновлюємо значення слайдера
}

// Функція для відтворення відео
void XVideoUI::Play()
{
	ui.pauseButton->show(); // Показуємо кнопку паузи
	ui.pauseButton->setGeometry(ui.playButton->geometry()); // Переміщаємо кнопку паузи на місце кнопки відтворення
	XVideoThread::Get()->Play(); // Запускаємо відтворення відео
	ui.playButton->hide(); // Ховаємо кнопку відтворення
}

// Функція для паузи відео
void XVideoUI::Pause()
{
	ui.playButton->show(); // Показуємо кнопку відтворення
	ui.pauseButton->hide(); // Ховаємо кнопку паузи
	XVideoThread::Get()->Pause(); // Ставимо відео на паузу
}

// Відкриваємо файл відео
void XVideoUI::Open()
{
	QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("Виберіть відеофайл")); // Відкриваємо діалог для вибору файлу
	if (name.isEmpty()) return; // Якщо файл не вибрано, виходимо
	string file = name.toLocal8Bit().data(); // Перетворюємо ім'я файлу в стандартний рядок
	if (!XVideoThread::Get()->Open(file)) // Відкриваємо відеофайл у потоці
	{
		QMessageBox::information(this, "error", name + " open failed!"); // Повідомляємо про помилку
		return;
	}

	Play(); // Починаємо відтворення
}

// Функція для обробки натискання на слайдер
void XVideoUI::SliderPress()
{
	pressSlider = true; // Позначаємо, що слайдер натиснуто
}

// Функція для обробки відпускання слайдера
void XVideoUI::SliderRelease()
{
	pressSlider = false; // Позначаємо, що слайдер відпущено
}

// Встановлення початкової позиції відтворення
void XVideoUI::Left(int pos)
{
	XVideoThread::Get()->SetBegin((double)pos / 1000.); // Встановлюємо початок відео
	SetPos(pos); // Оновлюємо поточну позицію
}

// Встановлення кінцевої позиції відтворення
void XVideoUI::Right(int pos)
{
	XVideoThread::Get()->SetEnd((double)pos / 1000.); // Встановлюємо кінець відео
}

// Встановлення точної позиції в відео
void XVideoUI::SetPos(int pos)
{
	XVideoThread::Get()->Seek((double)pos / 1000.); // Переміщаємо відео в точку
}

// Об'єднання відео
void XVideoUI::Merge()
{
	isMark = false;
	isBlend = false;
	isMerge = false;
	QString name = QFileDialog::getOpenFileName(this, "select video:"); // Вибір відео для об'єднання
	if (name.isEmpty()) return;
	std::string file = name.toLocal8Bit().data(); // Перетворюємо шлях до файлу
	isMerge = XVideoThread::Get()->Open2(file); // Відкриваємо друге відео для об'єднання
}

// Змішування відео
void XVideoUI::Blend()
{
	isMark = false;
	isBlend = false;
	isMerge = false;
	QString name = QFileDialog::getOpenFileName(this, "select video:"); // Вибір відео для змішування
	if (name.isEmpty()) return;
	std::string file = name.toLocal8Bit().data(); // Перетворюємо шлях до файлу
	isBlend = XVideoThread::Get()->Open2(file); // Відкриваємо відео для змішування
}

// Маркування відео
void XVideoUI::Mark()
{
	isMark = false;
	isBlend = false;
	isMerge = false;
	QString name = QFileDialog::getOpenFileName(this, "select image:"); // Вибір зображення для маркування
	if (name.isEmpty()) return;
	std::string file = name.toLocal8Bit().data(); // Перетворюємо шлях до файлу
	cv::Mat mark = imread(file); // Зчитуємо зображення
	if (mark.empty()) return; // Якщо зображення не вдалося зчитати, виходимо
	XVideoThread::Get()->SetMark(mark); // Встановлюємо маркування
	isMark = true; // Позначаємо, що маркування активне
}

// Завершення експорту відео
void XVideoUI::ExportEnd()
{
	isExport = false; // Встановлюємо статус експорту в false
	ui.exportButton->setText("Start Export"); // Змінюємо текст кнопки експорту на "Start Export"

	// Отримуємо початкові та кінцеві моменти для експорту
	string src = XVideoThread::Get()->src1file;
	string des = XVideoThread::Get()->desFile;
	int ss = XVideoThread::Get()->totalMs * ((double)ui.left->value() / 1000.); // Початковий момент
	int end = XVideoThread::Get()->totalMs * ((double)ui.right->value() / 1000.); // Кінцевий момент
	int t = end - ss; // Тривалість експортованого відео

	// Експортуємо аудіо та відео
	XAudio::Get()->ExportA(src, src + ".mp3", ss, t);
	string tmp = des + ".avi";
	QFile::remove(tmp.c_str()); // Видаляємо тимчасовий файл
	QFile::rename(des.c_str(), tmp.c_str()); // Переіменовуємо файл
	XAudio::Get()->Merge(tmp, src + ".mp3", des); // Об'єднуємо відео з аудіо
}

// Початок або зупинка експорту
void XVideoUI::Export()
{
	if (isExport) // Якщо експорт вже в процесі, зупиняємо його
	{
		XVideoThread::Get()->StopSave(); // Зупиняємо збереження
		isExport = false; // Встановлюємо статус експорту в false
		ui.exportButton->setText("Start Export"); // Змінюємо текст кнопки на "Start Export"
		return;
	}
	QString name = QFileDialog::getSaveFileName(this, "save", "out1.avi"); // Відкриваємо діалог збереження
	if (name.isEmpty()) return; // Якщо файл не вибрано, виходимо
	std::string filename = name.toLocal8Bit().data(); // Отримуємо шлях до файлу
	int w = ui.width->value();
	int h = ui.height->value();
	if (XVideoThread::Get()->StartSave(filename, w, h, isColor)) // Починаємо експорт
	{
		isExport = true;
		ui.exportButton->setText("Stop Export"); // Змінюємо текст кнопки на "Stop Export"
	}
}

// Налаштування фільтрів та параметрів відео
void XVideoUI::Set()
{
	XFilter::Get()->Clear(); // Очищаємо фільтри
	isColor = true; // За замовчуванням відео кольорове
	bool isClip = false;
	double cx = ui.cx->value();
	double cy = ui.cy->value();
	double cw = ui.cw->value();
	double ch = ui.ch->value();
	if (cx + cy + cw + ch > 0.0001) // Якщо є параметри обрізки
	{
		isClip = true;
		XFilter::Get()->Add(XTask{ XTASK_CLIP, { cx,cy,cw,ch } }); // Додаємо задачу обрізки
		double w = XVideoThread::Get()->width;
		double h = XVideoThread::Get()->height;
		XFilter::Get()->Add(XTask{ XTASK_RESIZE, { w, h } }); // Додаємо задачу зміни розміру
	}

	bool isPy = false;
	int down = ui.pydown->value();
	int up = ui.pyup->value();
	if (up > 0) // Якщо є збільшення розміру
	{
		isPy = true;
		XFilter::Get()->Add(XTask{ XTASK_PYUP, { (double)up } }); // Додаємо задачу збільшення
		int w = XVideoThread::Get()->width;
		int h = XVideoThread::Get()->height;
		for (int i = 0; i < up; i++)
		{
			h = h * 2;
			w = w * 2;
		}
		ui.width->setValue(w);
		ui.height->setValue(h);
	}
	if (down > 0) // Якщо є зменшення розміру
	{
		isPy = true;
		XFilter::Get()->Add(XTask{ XTASK_PYDOWN, { (double)down } }); // Додаємо задачу зменшення
		int w = XVideoThread::Get()->width;
		int h = XVideoThread::Get()->height;
		for (int i = 0; i < up; i++)
		{
			h = h / 2;
			w = w / 2;
		}
		ui.width->setValue(w);
		ui.height->setValue(h);
	}

	// Якщо не застосовуються фільтри обрізки або масштабування, встановлюємо розмір
	double w = ui.width->value();
	double h = ui.height->value();
	if (!isMerge && !isClip && !isPy && ui.width->value() > 0 && ui.height->value() > 0)
	{
		XFilter::Get()->Add(XTask{ XTASK_RESIZE, { w, h } });
	}

	// Налаштування яскравості та контрасту
	if (ui.bright->value() > 0 || ui.contrast->value() > 1)
	{
		XFilter::Get()->Add(XTask{ XTASK_GAIN, { (double)ui.bright->value(), ui.contrast->value() } });
	}

	// Налаштування кольору
	if (ui.color->currentIndex() == 1)
	{
		XFilter::Get()->Add(XTask{ XTASK_GRAY }); // Перетворення в чорно-біле
		isColor = false;
	}

	// Налаштування обертання
	if (ui.rotate->currentIndex() == 1)
	{
		XFilter::Get()->Add(XTask{ XTASK_ROTATE90 }); // Поворот на 90 градусів
	}
	else if (ui.rotate->currentIndex() == 2)
	{
		XFilter::Get()->Add(XTask{ XTASK_ROTATE180 }); // Поворот на 180 градусів
	}
	else if (ui.rotate->currentIndex() == 3)
	{
		XFilter::Get()->Add(XTask{ XTASK_ROTATE270 }); // Поворот на 270 градусів
	}

	// Налаштування відзеркалення
	if (ui.flip->currentIndex() == 1)
	{
		XFilter::Get()->Add(XTask{ XTASK_FLIPX }); // Віддзеркалення по осі X
	}
	else if (ui.flip->currentIndex() == 2)
	{
		XFilter::Get()->Add(XTask{ XTASK_FLIPY }); // Віддзеркалення по осі Y
	}
	else if (ui.flip->currentIndex() == 3)
	{
		XFilter::Get()->Add(XTask{ XTASK_FLIPXY }); // Віддзеркалення по обох осях
	}

	// Налаштування маркування
	if (isMark)
	{
		double x = ui.mx->value();
		double y = ui.my->value();
		double a = ui.ma->value();
		XFilter::Get()->Add(XTask{ XTASK_MARK, {x,y,a} }); // Додаємо задачу маркування
	}

	// Налаштування змішування
	if (isBlend)
	{
		double a = ui.ba->value();
		XFilter::Get()->Add(XTask{ XTASK_BLEND, { a } }); // Додаємо задачу змішування
	}

	// Налаштування об'єднання відео
	if (isMerge)
	{
		double h2 = XVideoThread::Get()->height2;
		double h1 = XVideoThread::Get()->height;
		int w = XVideoThread::Get()->width2 * (h2 / h1); // Обчислюємо нову ширину для об'єднання
		XFilter::Get()->Add(XTask{ XTASK_MERGE }); // Додаємо задачу об'єднання
		ui.width->setValue(XVideoThread::Get()->width + w); // Оновлюємо ширину
		ui.height->setValue(h1); // Оновлюємо висоту
	}
}