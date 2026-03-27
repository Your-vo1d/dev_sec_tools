#ifndef ILOG_H
#define ILOG_H

#pragma once

#include <string>
#include <iostream>
#include <QTime>
#include <QObject>
#include "IFileContainer.h"

//Определяет методы для обработки событий файловой системы.
class ILog : public QObject
{
    Q_OBJECT

public:
    virtual ~ILog() = default;

    //Записывает сообщение в лог.

    virtual void log(const std::string &data) = 0;

public slots:

    //Срабатывает при обнаружении файла.
    virtual void onFileExistence(IFileContainer *container, int index) = 0;

    //Срабатывает при обновлении файла.
    virtual void onFileUpdate(IFileContainer *container, int index) = 0;

    //Срабатывает при удалении файла.
    virtual void onFileRemoval(IFileContainer *container, int index) = 0;

    //Срабатывает в конце цикла обработки файлов
    virtual void onCycleEnd() = 0;
};


class ConsoleLog : public ILog
{
    Q_OBJECT

private:

    //Текущее время (используется для вывода временной метки)
    QTime m_time;

    //Флаг вывода времени перед сообщением
    bool m_logTime = false;

    //Преобразует qint64 в std::string
    static std::string qint64ToString(qint64 value);

public:

    ~ConsoleLog() = default;

    //Конструктор
    explicit ConsoleLog(bool logTime);

    //Вывод сообщения в консоль
    void log(const std::string &data) override;

    //Очистка консоли
    void clearConsole();

public slots:

    //Обработка события обнаружения файла
    void onFileExistence(IFileContainer *container, int index) override;

    //Обработка события обновления файла
    void onFileUpdate(IFileContainer *container, int index) override;

    //Обработка события удаления файла
    void onFileRemoval(IFileContainer *container, int index) override;

    //Завершение цикла обработки
    void onCycleEnd() override;
};

#endif // ILOG_H
