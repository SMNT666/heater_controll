#ifndef HEATERS_HEATER_H
#define HEATERS_HEATER_H

#include <deque>
#include <functional>

#include "variables_description.h"

using HeatSetter = std::function<void (HeaterNum, bool)>;

class Heater {
public:
    Heater();

    /**
     * Устанавливает заданную мощность на нагреватель.
     *
     * @note Обновление текущей мощности происходит только при вызове метода _update().
     *       Если до вызова _update() мощность устанавливалась несколько раз,
     *       в нагреватель будет установлена округленное среднее значение всех установленных мощностей.
     *
     * @param power - устанавливаемая мощность в процентах (от нуля до 100)
     */
    void setPower(Power power);

    /**
     * Возвращает текущую мощность нагревателя или мощность установленную в прошлом.
     *
     * @note - хранит историю за последние 10 минут (600 секунд)
     *
     * @note - не учитывает текущую (т.е. ещё не завершённую) секунду
     *
     * @param timeOffset - момент времени, для которого
     * 		  запрашивается мощность (0: текущая мощность, -1: мощность,
     * 		  которая была на нагревателе одну секунду назад и т.д.)
     */
    Power getPower(unsigned short timeOffset = 0) const;

    /**
     * Возвращает мощность установленную в текущий полупериод
     */
    Power getCurrentPower() const;

    /**
     * Устанавливает заданное состояние на нагреватель
     *
     * @param state - устанавливаемое состояние
     */
    void setState(bool state);

    /**
     * Возвращает состояние нагревателя в прошлый полупериод
     */
    bool getLastState() const;

    /**
     * Данный метод служит для обновления:
     *    - обновление истории устанавливаемых мощностей.
     *
     * @note Данный метод необходимо 1 раз в 1 секунду
     */
    void update();

private:
    /**
     * Максимально возможная мощность в процентах
     */
    static const short MAXIMUM_POWER = 100;
    /**
     * Максимальный лимит времени хранения истории мощности
     *
     * @note Единицы измерения - секунда
     */
    static const short TIME_LIMIT = 600;

private:
    Power _power = 0;
    /**
     * Количество включений нагревателей за 1 секунду
     * @note Количество вызовов функции HeatSetter с аргументом True
     */
    short _setTrueStateCount = 0;
    bool _state = false;
    bool _lastState = false;
    std::deque<Power> _log;
};

#endif //HEATERS_HEATER_H
