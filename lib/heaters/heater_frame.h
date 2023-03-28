#ifndef HEATERS_HEATER_FRAME_H
#define HEATERS_HEATER_FRAME_H

#include <vector>

#include "variables_description.h"
#include "heater.h"

/**
 * Для обеспечения включения нагревателей определенное количество раз за секунду (в соответствии с мощностью)
 * Используется следующее решение:
 * Представим 1 секунду времени, как вектор размером 100 элементов
 * (1c = 100 полупериодов)
 * vector<HeatFrame> heat_frames;
 *
 * Каждый элемент этого вектора содержит HeatFrame
 * HeatFrame - хранит номера нагревателей, которые должны быть включены в данный полупериод.
 *
 * Тогда при обращении к элементу вектора кадров с индексом равным необходимому полупериоду
 * получим номера включенных в данный момент времени нагревателей
 */
class HeatFrame {
public:
    HeatFrame() = default;

    void addHeater(const HeaterNum& heater);

    short getEvenHeatersCount() const;

    short getOddHeatersCount() const;

    std::vector<HeaterNum>::iterator begin();

    std::vector<HeaterNum>::iterator end();

private:
    std::vector<HeaterNum> _heaters;

    short _evenHeatersCount = 0;
    short _oddHeatersCount = 0;
};

#endif //HEATERS_HEATER_FRAME_H