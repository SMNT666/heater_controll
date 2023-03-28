#include "heater_frame.h"

void HeatFrame::addHeater(const HeaterNum &heater) {
    _heaters.push_back(heater);

    if(heater % 2 == 0)
        ++_evenHeatersCount;
    else
        ++_oddHeatersCount;
}

short HeatFrame::getEvenHeatersCount() const {
    return _evenHeatersCount;
}

short HeatFrame::getOddHeatersCount() const {
    return _oddHeatersCount;
}

std::vector<HeaterNum>::iterator HeatFrame::begin() {
    return _heaters.begin();
}

std::vector<HeaterNum>::iterator HeatFrame::end() {
    return _heaters.end();
}