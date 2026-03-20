#ifndef IFILEREFRESHER_H
#define IFILEREFRESHER_H

#include <chrono>
#include <thread>
#include <time.h>
class IFileRefresher
{
public:
    virtual ~IFileRefresher() = default;
    virtual void refresh() = 0;
};

class FrequencyRefresher : public IFileRefresher
{
  private:
    float _refreshRate = 1000.0f; // Частота в Гц (раз в секунду)

  public:
    explicit FrequencyRefresher(float refreshRate)
        : _refreshRate(refreshRate)
    {
        // Защита от деления на ноль и отрицательных значений
        if (_refreshRate <= 0.0f) {
            _refreshRate = 1.0f; 
        }
    }

    void refresh() override {
        // Вычисляем задержку в миллисекундах: 1000 мс / частота
        auto ms = static_cast<int>(1000.0f / _refreshRate);
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
};


#endif // IFILEREFRESHER_H
