#pragma once
#include <QOpenGLWidget>
#include <opencv2/core.hpp>
class XVideoWidget :public QOpenGLWidget
{
	Q_OBJECT

public:
	// Конструктор класу XVideoWidget
	// p - батьківський віджет
	XVideoWidget(QWidget* p);

	// Обробляє подію малювання
	// e - подія малювання
	void paintEvent(QPaintEvent* e);

	// Деструктор класу XVideoWidget
	virtual ~XVideoWidget();

public slots:

	// Встановлює зображення для відображення
	// mat - зображення (cv::Mat)
	void SetImage(cv::Mat mat);

protected:
	QImage img; // // Зображення, що буде відображено
};