#ifndef HEATERS
#define HEATERS

#include <stack>
#include <unordered_map>
#include <vector>

#include "variables_description.h"
#include "heater.h"
#include "heater_frame.h"

class IHeaters {
public:
    virtual ~IHeaters() = default;

    virtual void setPower(HeaterNum, Power) = 0;

    virtual void getMaxNumOfTurnedHeatersAfterPowerChange(HeaterNum, Power,
                                                          unsigned int&, unsigned int&) = 0;

    virtual Power getPower(HeaterNum, unsigned short timeOffset = 0) = 0;

    virtual bool getLastSemiPeriodState(HeaterNum) = 0;
};

class Heaters : public IHeaters {
public:
    /**
     * @param heatersNum - количество нагревателей
     *
     * @param setHeaterStateFn - функция для установки состояния
     * 		  нагревателя с указанным номером от нуля (true - включить,
     * 		  false - выключить)
     */
    Heaters(HeaterNum heatersNum,
            HeatSetter setHeaterStateFn);

    /**
     * Устанавливает заданную мощность на нагреватель
     *
     * @param heater - нагреватель, мощность которого нужно изменить
     *
     * @param power - устанавливаемая мощность в процентах (от нуля до 100)
     */
    void setPower(HeaterNum heater, Power power) override;

    /**
     * Вычисляет максимальную суммарную мощность,
     * которая будет выделяться на нагревателях в любой момент времени,
     * если установить такую мощность на заданный нагреватель
     *
     * @param[in] heater - нагреватель, мощность которого нужно изменить
     *
     * @param[in] power - мощность в процентах (от нуля до 100)
     *
     * @param[out] top - максимальное количество одновременно работающих
     * 					 ВЕРХНИХ нагревателей
     *
     * @param[out] bot - максимальное количество одновременно работающих
     * 					 НИЖНИХ нагревателей
     *
     */
    void getMaxNumOfTurnedHeatersAfterPowerChange(
            HeaterNum heater, Power power,
            unsigned int& top, unsigned int& bot) override;

    /**
     * Возвращает текущую мощность нагревателя или мощность,
     * установленную в прошлом
     *
     * @note - хранит историю за последние 10 минут (600 секунд)
     *
     * @note - не учитывает текущую (т.е. ещё не завершённую) секунду
     *
     * @param heater - нагреватель, мощность которого запрашивается
     *
     * @param timeOffset - момент времени, для которого
     * 		  запрашивается мощность (0: текущая мощность, -1: мощность,
     * 		  которая была на нагревателе одну секунду назад и т.д.)
     */
    Power getPower(HeaterNum heater, unsigned short timeOffset = 0) override;

    /**
     * Возвращает состояние нагревателя в прошлый полупериод
     *
     * @return - состояние нагревателя в прошлый полупериод
     */
    bool getLastSemiPeriodState(HeaterNum heaterNum) override;

    /**
     * Информирует модуль о переходе через ноль
     *
     * @note Если не вызывать этот метод, мощности на нагревателях
     * перестанут изменяться
     */
    void zeroCrossed();

private:
    /**
     * Возвращает максимальное количество включенных нижних и верхних нагревателей
     * @param heatFrames - вектор кадров нагревания
     * @return first - четные, second - нечётные
     */
    static std::pair<short, short> _getMaximumEvenOddHeaters(const std::vector<HeatFrame>& heatFrames);

private:
    /**
     * Выполняет обновление схемы нагревания
     * в соответствии с установленными мощностями
     */
    void _updateHeatFrames();

    /**
     * Возвращает контейнер кадров нагревания
     * @param heaters - контейнер нагревателей
     * @return вектор кадров нагревании
     */
    std::vector<HeatFrame> _getHeatFrames(const std::vector<Heater>& heaters) const;

     /**
     * Метод обновляет состояние всех нагревателей
     */
    void _update();

    /**
     * Данный метод обеспечивает включение и отключение нагревателей.
     * Вызывается каждый полупериод.
     */
    void _heating();
    /**
     * Данный метод проверяет сколько времени работал модуль
     * @return Возвращает true, если прошла 1 секунда, иначе false
     */
    bool _secondLeft() const;

private:
    /**
     * Количество полупериодов в 1 секунде
     */
    const short FRAME_COUNT = 100;

private:
    std::vector<Heater> _heaters;
    HeatSetter _setHeaterState;
    std::vector<HeatFrame> _heatFrames;
    std::unordered_map<HeaterNum, bool> _heatersNumToState;
    Frame _currentFrame = 0;
};
#endif // HEATERS