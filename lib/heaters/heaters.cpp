#include "heaters.h"

/**
 * Принцип управления мощностью:
 * Если нужно подать 40% мощности на нагреватель,
 * он должен быть включён 40 полупериодов из 100
 * (предполагается, что частота сети 50Гц, т.е. в секунде 100 полупериодов)
 *
 * Т.е., чтобы подать 40% мощности на нулевой нагреватель,
 * нужно в начале новой секунды включить нагреватель (setHeaterStateFn(0, true)),
 * спустя 40 полупериодов (через 40 вызовов метода zeroCrossed) выключить его.
 * Через 60 полупериодов начнётся новая секунда,
 * снова включаем нагреватель и т.д.
 *
 * Не обязательно держать нагреватель включённым непрерывно. Например, можно
 * включать его каждый чётный полупериод и выключать каждый нечётный
 * (лишь бы он был включён ровно 40 полупериодов из 100)
 */

Heaters::Heaters(HeaterNum heatersNum, HeatSetter setHeaterStateFn)
        : _heaters(heatersNum), _setHeaterState(std::move(setHeaterStateFn)), _heatFrames(FRAME_COUNT) {
    //Устанавливаем начальное состояние нагревателей - выкл
    for(HeaterNum num = 0; num < heatersNum; ++num) {
        _heatersNumToState[num] = false;
    }
}

void Heaters::setPower(HeaterNum heater, Power power) {
    _heaters[heater].setPower(power);
    _updateHeatFrames();
}

void Heaters::_updateHeatFrames() {
    _heatFrames = std::move(_getHeatFrames(_heaters));
}

void Heaters::getMaxNumOfTurnedHeatersAfterPowerChange(HeaterNum heater,Power power,
                                                       unsigned int &top, unsigned int &bot) {
    std::vector<Heater> heaters = _heaters;

    heaters[heater].setPower(power);

    auto heatFrames = std::move(_getHeatFrames(heaters));
    auto evenOddHeaters = _getMaximumEvenOddHeaters(heatFrames);

    top = evenOddHeaters.first;
    bot = evenOddHeaters.second;
}

void Heaters::zeroCrossed() {
    _heating();

    ++_currentFrame;

    if(_secondLeft()) {
        _currentFrame = 0;
        _update();
    }
}

void Heaters::_heating() {
    // Для каждого нагревателя устанавливаем состояние выкл
    for(auto& pairNumAndState: _heatersNumToState) {
        pairNumAndState.second = false;
    }

    // В текущем фрейме находим Включенные нагреватели
    for(HeaterNum heaterNum: _heatFrames[_currentFrame]) {
        _heatersNumToState[heaterNum] = true;
    }

    // В соответствии с мапой состояний вкл/выкл нагреватели
    for(auto& pairNumAndState: _heatersNumToState) {
        auto heaterNum = pairNumAndState.first;
        auto state = pairNumAndState.second;

        _heaters[heaterNum].setState(state);
        _setHeaterState(heaterNum, state);
    }
}

bool Heaters::getLastSemiPeriodState(HeaterNum heaterNum) {
    return _heaters[heaterNum].getLastState();
}

std::vector<HeatFrame> Heaters::_getHeatFrames(const std::vector<Heater> &heaters) const {
    std::vector<HeatFrame> heatFrames(FRAME_COUNT);

    Frame frame = 0;

    for(HeaterNum heaterNum = 0; heaterNum < heaters.size(); ++heaterNum) {
        for(Power power = 0; power < heaters[heaterNum].getCurrentPower(); ++power) {
            heatFrames[frame].addHeater(heaterNum);

            ++frame;

            if(frame >= FRAME_COUNT) frame = 0;
        }
    }

    return heatFrames;
}

std::pair<short, short> Heaters::_getMaximumEvenOddHeaters(const std::vector<HeatFrame>& heatFrames) {
    short maxEvenHeaters = 0;
    short maxOddHeaters = 0;

    for(const HeatFrame& frame: heatFrames) {
        if (frame.getOddHeatersCount() > maxOddHeaters) {
            maxOddHeaters = frame.getOddHeatersCount();
        }

        if (frame.getEvenHeatersCount() > maxEvenHeaters) {
            maxEvenHeaters = frame.getEvenHeatersCount();
        }
    }

    return {maxEvenHeaters, maxOddHeaters};
}

void Heaters::_update() {
    for(Heater& heater: _heaters) {
        heater.update();
    }
}

Power Heaters::getPower(HeaterNum heater, unsigned short timeOffset) {
    if(heater < _heaters.size()) {
        return _heaters[heater].getPower(timeOffset);
    } else {
        return 0;
    }
}

bool Heaters::_secondLeft() const {
    return _currentFrame == FRAME_COUNT;
}
