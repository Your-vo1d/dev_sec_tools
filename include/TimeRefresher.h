#ifndef TIMEREFRESHER_H
#define TIMEREFRESHER_H

#include "IFileRefresher.h"

class TimeRefresher : public IFileRefresher
{
private:
    float _refreshRate = 1000.0f;  // Частота обновления (раз в секунду)

public:
    // Конструктор с валидацией параметра
    explicit TimeRefresher(float refreshRate);

    // Ожидание до следующего цикла обновления
    void refresh() override;
};

#endif // TIMEREFRESHER_H
