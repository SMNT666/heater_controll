#include "gtest/gtest.h"

#include "heaters.h"

/**
 * Ничего не изменяет, если мощность больше 100
 */
TEST(SetPower, does_not_change_if_power_101) {
    bool heater_state = false;
    short heater_num = -1;

    auto setter = [&heater_num, &heater_state](int num, bool state) {
        heater_state = state;
        heater_num = num;
    };

    Heaters heaters(1, setter);

    heaters.setPower(0, 101);

    Power p = 0;
    for (int i = 0; i < 100; ++i) {
        heaters.zeroCrossed();
        if (heater_state) {
            ++p;
        }
    }

    ASSERT_EQ(p, 0);
}


/**
 * Установленная мощность не изменяется с течением времени
 */
TEST(SetPower, power_stays_same_if_new_power_was_not_set) {
    bool heater_state = false;
    short heater_num = -1;

    auto setter = [&heater_num, &heater_state](int num, bool state) {
        heater_state = state;
        heater_num = num;
    };

    Heaters heaters(1, setter);

    // Продержать мощность 24 в течение секунды
    heaters.setPower(0, 24);
    for (int i = 0; i < 100; ++i) {
        heaters.zeroCrossed();
    }

    // Посчитать мощность, которая выделится за следующую секунду
    Power p = 0;
    for (int i = 0; i < 100; ++i) {
        heaters.zeroCrossed();
        if (heater_state) {
            ++p;
        }
    }
    ASSERT_EQ(p, 24);
}


/**
 * Мощность должна изменятся в течение секунды
 */
TEST(SetPower, change_during_second) {
    bool heater_state = false;
    short heater_num = -1;

    auto setter = [&heater_num, &heater_state](int num, bool state) {
        heater_state = state;
        heater_num = num;
    };

    Heaters heaters(1, setter);

    for (int i = 0; i < 20; ++i) {
        heaters.zeroCrossed();
        ASSERT_EQ(heater_state, false);
    }
    heaters.setPower(0, 100);

    for (int i = 0; i < 80; ++i) {
        heaters.zeroCrossed();
        ASSERT_EQ(heater_state, true);
    }
}


/**
 * Для нагревателя, номер которого больше либо равен числу нагревателей
 * (отсчёт от нуля), возвращает нулевую мощность
 */
TEST(GetPower, returns_0_if_heaters_num_GE_than_heatersNum) {
    auto setter = [](int, bool) {};

    Heaters heaters(1, setter);

    ASSERT_EQ(heaters.getPower(100), 0);
}


/**
 * Устанавливает заданную мощность на нагреватель
 */
TEST(SetPower, power_42_heater_is_on_42_times_within_100_semi_periods) {
    bool heater_state = false;
    short heater_num = -1;

    auto setter = [&heater_num, &heater_state](int num, bool state) {
        heater_state = state;
        heater_num = num;
    };

    Heaters heaters(1, setter);

    heaters.setPower(0, 42);

    // Посчитать мощность, которая выделится за следующую секунду
    Power p = 0;
    for (int i = 0; i < 100; ++i) {
        heaters.zeroCrossed();
        if (heater_state) {
            ++p;
        }
    }
    ASSERT_EQ(p, 42);
}


/**
 * Если вызывается без оффсета
 * Возвращает мощность за последнюю законченную секунду
 * (т.е. не учитывает мощности текущей незавершённой секунды)
 */
TEST(GetPower, without_offset_returns_last_second_power) {
    auto setter = [](int, bool) {};
    Heaters heaters(1, setter);

    // Устанавливаем мощность ожидаем 1 с
    heaters.setPower(0, 10);

    for(int i = 0; i < 100; ++i) {
        heaters.zeroCrossed();
    }

    // Устанавливаем разные мощности, но не завершаем секунду
    for(int i = 0; i < 99; ++i) {
        heaters.setPower(0, i);
        heaters.zeroCrossed();
    }

    // Ожидаемый результат - мощность установленная на прошлой полной секунде
    ASSERT_EQ(heaters.getPower(0), 10);
}


/**
 * Если оффсет больше числа прошедших полупериодов, возвращает нулевую мощность
 */
TEST(GetPower, returns_0_if_offset_G_than_amount_of_zeroCrossed_calls) {
    auto setter = [](int, bool) {};

    Heaters heaters(1, setter);
    heaters.setPower(0, 100);

    for(int i = 0; i < 2 * 100; ++i) {
        heaters.zeroCrossed();
    }

    ASSERT_EQ(heaters.getPower(0, 3), 0);
}


/**
 * Если мощность менялась в течение секунды, возвращается округлённая средняя
 * мощность
 */
TEST(GetPower, returns_rounded_mean_if_power_changed_within_second) {
    bool heater_state = false;
    short heater_num = -1;

    auto setter = [&heater_num, &heater_state](int num, bool state) {
        heater_state = state;
        heater_num = num;
    };

    Heaters heaters(1, setter);
    Power p = 0;

    heaters.setPower(0, 10);
    for(int i = 0; i < 20; ++i) {
        heaters.zeroCrossed();
        if (heater_state) {
            ++p;
        }
    }

    heaters.setPower(0, 70);
    for(int i = 0; i < 40; ++i) {
        heaters.zeroCrossed();
        if (heater_state) {
            ++p;
        }
    }

    // Завершаем секунду
    for (int i = 0; i < 40; ++i) {
        heaters.zeroCrossed();
        if (heater_state) {
            ++p;
        }
    }

    heaters.setPower(0, 0);
    ASSERT_EQ(heaters.getPower(0), p);
}


/**
 * Хранит историю установленных мощностей за последние 10 минут
 */
TEST(GetPower, keeps_history_of_last_10_minuets_powers) {
    auto setter = [](int, bool) {};

    Heaters heaters(1, setter);

    heaters.setPower(0, 100);

    for(int i = 0; i < 60 * 100; ++i) {
        heaters.zeroCrossed();
    }

    heaters.setPower(0,50);

    for(int i = 0; i < 9 * 60 * 100; ++i) {
        heaters.zeroCrossed();
    }

    ASSERT_EQ(heaters.getPower(0, 599), 100);
    ASSERT_EQ(heaters.getPower(0, 20), 50);
}


/**
 * Возвращает то же состояние, которое установил на нагреватель в прошлый
 * полупериод
 */
TEST(getLastSemiPeriodState, returns_state_set_power_called_with) {

    bool heater_state = false;
    short heater_num = -1;

    auto setter = [&heater_num, &heater_state](int num, bool state) {
        heater_state = state;
        heater_num = num;
    };

    Heaters heaters(1, setter);

    heaters.setPower(0, 28);

    bool last_state = false;
    for(int time = 0; time < 100; ++time) {
        last_state = heater_state;
        heaters.zeroCrossed();
        ASSERT_EQ(last_state, heaters.getLastSemiPeriodState(0));
    }
}


/**
 * Если суммарная мощность ноль, не работает ни один нагреватель
 */
TEST(getMaxNumOfTurnedHeatersAfterPowerChange,
     set_power_0_returns_top_0_bot_0) {
    auto setter = [](int, bool) {};

    Heaters heaters(42, setter);

    unsigned int top = 0;
    unsigned int bot = 0;

    heaters.getMaxNumOfTurnedHeatersAfterPowerChange(0,0, top, bot);

    ASSERT_EQ(top, 0);
    ASSERT_EQ(bot, 0);
}


/**
 * Даже при минимальной мощности число включённых нагревателей увеличивается
 */
TEST(getMaxNumOfTurnedHeatersAfterPowerChange,
     heaters_0_and_1_power_1_top_1_bot_1) {
    auto setter = [](int, bool) {};

    Heaters heaters(42, setter);
    unsigned int top = 0;
    unsigned int bot = 0;

    heaters.setPower(0, 1);
    heaters.getMaxNumOfTurnedHeatersAfterPowerChange(1, 1, top, bot);

    ASSERT_EQ(top, 1);
    ASSERT_EQ(bot, 1);
}


/**
 * Учитывает умное управление
 */
TEST(getMaxNumOfTurnedHeatersAfterPowerChange,
     _9_heaters_with_power_10_ask_power_10_to_10th_returns_top_1_bot_0) {
    auto setter = [](int, bool) {};

    Heaters heaters(40, setter);

    unsigned int top = 0;
    unsigned int bot = 0;

    short index = 0;
    short heaters_count = 0;

    // Устанавливаем мощность на 9 нагревателей, с чётными номерами (верхние)
    while(heaters_count < 9) {
        if(index % 2 == 0) {
            heaters.setPower(index, 10);
        }
        ++heaters_count;
    }

    heaters.getMaxNumOfTurnedHeatersAfterPowerChange(18, 10, top, bot);

    ASSERT_EQ(top, 1);
    ASSERT_EQ(bot, 0);
}