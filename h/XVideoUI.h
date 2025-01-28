#pragma once

#include <QtWidgets/QWidget>
#include "ui_XVideoUI.h"

class XVideoUI : public QWidget
{
    Q_OBJECT

public:
    // Конструктор класу XVideoUI
    // parent - батьківський віджет
    XVideoUI(QWidget* parent = Q_NULLPTR);

protected:
    // Обробляє події таймера
    // e - подія таймера
    void timerEvent(QTimerEvent* e) override;

public slots:
    // Відкриває відеофайл
    void Open();

    // Починає відтворення відео
    void Play();

    // Призупиняє відтворення відео
    void Pause();

    // Обробляє подію натискання на слайдер
    void SliderPress();

    // Обробляє подію відпускання слайдера
    void SliderRelease();

    // Встановлює позицію відтворення
    // pos - нова позиція відтворення
    void SetPos(int pos);

    // Встановлює ліву межу області відтворення
    // pos - позиція лівої межі
    void Left(int pos);

    // Встановлює праву межу області відтворення
    // pos - позиція правої межі
    void Right(int pos);

    // Виконує установку параметрів
    void Set();

    // Експортує відеофайл
    void Export();

    // Обробляє завершення експорту відеофайлу
    void ExportEnd();

    // Додає мітку на відео
    void Mark();

    // Виконує змішування відеофайлів
    void Blend();

    // Об'єднує відеофайли
    void Merge();

private:
    Ui::XVideoUIClass ui; // Інтерфейс користувача для класу XVideoUI
};