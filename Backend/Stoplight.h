#pragma once

#include <vector>

class Stoplight {
    int currentState;  // 1, 2, 3, 4 - состояния светофора
    int nextState;  // 0 - в данный момент состояние не определено

    std::vector<double> timeGoodWeather;  // Время работы светофора при хорошем состоянии внешней среды
    std::vector<double> timeBadWeather;  // Время работы светофора при плохом состоянии внешней среды

public:
    Stoplight() {
        currentState = 1;
        nextState    = 0;
    }

    // Циклический алгоритм смены состояния обслуживающего устройства
    void calculateNextState() {
        if (currentState % 4 != 0) {
            nextState = currentState + 1;
        } else {
            nextState = 1;
        }
    }

    void changeState() {
        currentState = nextState;
        nextState    = 0;
    }

    [[nodiscard]] int getCurrentState() const {
        return currentState;
    }

    void setCurrentState(const int& state) {
        currentState = state;
    }

    void setNextState(const int& state) {
        nextState = state;
    }

    void setTimeGoodWeather(const std::vector<double>& time) {
        timeGoodWeather = time;
    }

    [[nodiscard]] std::vector<double> getTimeGoodWeather() const {
        return timeGoodWeather;
    }

    void setTimeBadWeather(const std::vector<double>& time) {
        timeBadWeather = time;
    }

    [[nodiscard]] std::vector<double> getTimeBadWeather() const {
        return timeBadWeather;
    }

    [[nodiscard]] double getCycleTimeGW() const {
        return timeGoodWeather.at(0) + timeGoodWeather.at(1) + timeGoodWeather.at(2) + timeGoodWeather.at(3);
    }

    [[nodiscard]] double getCycleTimeBW() const {
        return timeBadWeather.at(0) + timeBadWeather.at(1) + timeBadWeather.at(2) + timeBadWeather.at(3);
    }

    [[nodiscard]] double getFirstStreamGreenTime(const int& environment) const {
        if (environment == 1) {
            return timeGoodWeather.at(0);
        } else {
            return timeBadWeather.at(0);
        }
    }

    [[nodiscard]] double getSecondStreamGreenTime(const int& environment) const {
        if (environment == 1) {
            return timeGoodWeather.at(2);
        } else {
            return timeBadWeather.at(2);
        }
    }
};
