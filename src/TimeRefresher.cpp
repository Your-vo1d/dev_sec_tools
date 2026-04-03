#include "TimeRefresher.h"
#include <chrono>
#include <thread>

// Конструктор
TimeRefresher::TimeRefresher(float refreshRate)
    : _refreshRate(refreshRate)
{
    // Защита от некорректных значений частоты
    if (_refreshRate <= 0.0f) {
        _refreshRate = 1.0f;
    }
}

// Реализация метода refresh()
void TimeRefresher::refresh()
{
    // Вычисляем задержку: 1000 мс / частота = интервал в миллисекундах
    // Пример: при 2 Гц → 1000/2 = 500 мс между вызовами
    auto milliseconds = static_cast<int>(1000.0f / _refreshRate);

    // Минимальная задержка — 1 мс, чтобы избежать busy-wait
    if (milliseconds < 1) {
        milliseconds = 1;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
