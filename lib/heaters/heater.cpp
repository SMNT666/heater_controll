#include "heater.h"

Heater::Heater() : _log(TIME_LIMIT, 0) {
}

void Heater::setPower(Power power) {
    if(power > MAXIMUM_POWER) return;

    _power = power;
}

Power Heater::getPower(unsigned short timeOffset) const {
    if(timeOffset < TIME_LIMIT) {
        if(timeOffset == 0) {
            return _log.front();
        } else {
            return _log[timeOffset];
        }
    } else {
        return 0;
    }
}

void Heater::setState(bool state) {
    _lastState = _state;

    if(state != _state)
        _state = state;

    if(state) {
        ++_setTrueStateCount;
    }
}

bool Heater::getLastState() const {
    return _lastState;
}

void Heater::update() {
    _log.push_front(_setTrueStateCount);
    _log.pop_back();

    _setTrueStateCount = 0;
}

Power Heater::getCurrentPower() const {
    return _power;
}
