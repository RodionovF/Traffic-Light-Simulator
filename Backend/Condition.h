#include "Stoplight.h"
#include "Environment.h"
#include "Stream.h"
#include <iostream>

using namespace std;

void updateOnArrive(Stream& stream) {
    stream.setKappa(stream.getKappa() + stream.getArrivedCarsForOneIteration());
    stream.setArrivedCarsStream(stream.getArrivedCarsStream() + stream.getArrivedCarsForOneIteration());
}

void updateOnLeave(Stream& stream, double& averageWaitingTime, const double& currentTime, double& sqrAverageTime,
                   double& averageVariationTime, double& timeLastCar) {
    std::queue<double> helpQueue;
    helpQueue          = stream.getArriveTime();
    timeLastCar        = currentTime - helpQueue.front();
    averageWaitingTime = averageWaitingTime * stream.getServicedCarsStream() + timeLastCar;
    sqrAverageTime     = sqrAverageTime * stream.getServicedCarsStream() + pow(timeLastCar, 2);

    helpQueue.pop();
    stream.setArriveTime(helpQueue);
    stream.setKappa(stream.getKappa() - 1);
    stream.setServicedCarsStream(stream.getServicedCarsStream() + 1);
    averageWaitingTime /= stream.getServicedCarsStream();
    sqrAverageTime /= stream.getServicedCarsStream();

    averageVariationTime = sqrAverageTime - pow(averageWaitingTime, 2);
}

void carArrive(bool& true_flag, bool& true_flagOnce, bool& false_first, bool& false_second, bool& false_third,
               Environments& environment, Stoplight& stoplight, Stream& stream, const double& currentTime) {
    // Последнее событие - уход заявки с потока
    false_first  = false;
    false_second = false;
    false_third  = false;
    true_flag    = true;

    // Включаем флаг проверки определения текущего времени для потока
    true_flagOnce = true;
    environment.setNextState(environment.getCurrentState());
    stoplight.setNextState(stoplight.getCurrentState());
    //if ((stream.getKappa() == 1) || (stream.getServiceTime() == 0)) {
    stream.setServiceTime(currentTime);
    //}
    stream.generateService(environment.getCurrentState());
}

bool timeless(const double& lastTime, const int& currEnvironment, const Stoplight& stoplight, const double& allTime) {
    if (currEnvironment == 1) {
        return lastTime < (stoplight.getTimeGoodWeather().at(stoplight.getCurrentState() - 1) + allTime);
    } else {
        return lastTime < (stoplight.getTimeBadWeather().at(stoplight.getCurrentState() - 1) + allTime);
    }
}

void updateData(const Stream& stream1, const Stream& stream2, double& averageQueue1, double& averageQueue2,
                int& counter, double& sqrAverageQueue1, double& sqrAverageQueue2,
                double& averageVariationQueue1, double& averageVariationQueue2) {
    averageQueue1    = averageQueue1 * counter + stream1.getKappa();
    averageQueue2    = averageQueue2 * counter + stream2.getKappa();
    sqrAverageQueue1 = sqrAverageQueue1 * counter + pow(stream1.getKappa(), 2);
    sqrAverageQueue2 = sqrAverageQueue2 * counter + pow(stream2.getKappa(), 2);

    counter++;
    averageQueue1 /= counter;
    averageQueue2 /= counter;
    sqrAverageQueue1 /= counter;
    sqrAverageQueue2 /= counter;

    averageVariationQueue1 = sqrAverageQueue1 - pow(averageQueue1, 2);
    averageVariationQueue2 = sqrAverageQueue2 - pow(averageQueue2, 2);
}

class Condition {
private:
    Environments environment;

    Stoplight stoplight;
    Stream    stream1;
    Stream    stream2;
    int       arrivedCars; // Общее число прибывших машин
    double    allTime; // Общее время
    double    currentTime; // Текущее время
    bool      flag1; // Последнее событие: true - пришла заявка по первому потоку
    bool      flag2; // Последнее событие: true - пришла заявка по второму потоку
    bool      flag3; // Последнее событие: true - ушла заявка по первому потоку
    bool      flag4; // Последнее событие: true - ушла заявка по второму потоку
    bool      flagOnce1;
    bool      flagOnce2;

    double averageQueue1; // Средняя очередь по первому потоку
    double averageQueue2; // Средняя очередь по второму потоку
    int    counter; // Счетчик циклов светофора
    double averageVariationQueue1; // Дисперия очереди по первому потоку
    double averageVariationQueue2; // Дисперсия очереди по второму потоку
    double sqrAverageQueue1; // Средняя очередь от квадрата по первому потоку
    double sqrAverageQueue2; // Средняя очередь от квадрата по второму потоку
    double averageWaitingTime1; // Среднее время ожидания по первому потоку
    double averageWaitingTime2; // Среднее время ожидания по второму потоку
    double averageVariationTime1; // Дисперсия времени ождиания по первому потоку
    double averageVariationTime2; // Дисперсия времени ождиания по второму потоку
    double sqrAverageTime1; // Среднее время от квадрата по первому потоку
    double sqrAverageTime2; // Среднее время от квадрата по второму потоку
    double timeBadWeather; // Общее время прибывания системы в плохом состоянии
    double timeGoodWeather; // Общее время прибывания системы в хорошем состоянии
    double firstStreamTime; // Время, отведенное под обслуживание первого потока
    double secondStreamTime; // Время, отведенное под обслуживание второго потока
    double mathLoaderFirstStream; // Аналитическая загрузка первого потока
    double mathLoaderSecondStream; // Аналитическая загрузка второго потока

    double timeLastCar1; // Время ожидания последней заявки по первому потоку
    double timeLastCar2; // Время ожидания последней заявки по второму потоку
    bool   isStationary; // Наличие стационарного режима в системе

public:
    Condition() : environment(), stoplight(), stream1(), stream2() {
        arrivedCars            = 0;
        allTime                = 0;
        currentTime            = 0;
        flag1                  = true;
        flag2                  = true;
        flag3                  = true;
        flag4                  = true;
        flagOnce1              = false;
        flagOnce2              = false;
        averageQueue1          = 0;
        averageQueue2          = 0;
        counter                = 0;
        averageVariationQueue1 = 0;
        averageVariationQueue2 = 0;
        sqrAverageQueue1       = 0;
        sqrAverageQueue2       = 0;
        averageWaitingTime1    = 0;
        averageWaitingTime2    = 0;
        averageVariationTime1  = 0;
        averageVariationTime2  = 0;
        sqrAverageTime1        = 0;
        sqrAverageTime2        = 0;
        timeBadWeather         = 0;
        timeGoodWeather        = 0;
        isStationary           = true;
        timeLastCar1           = 0;
        timeLastCar2           = 0;
        firstStreamTime        = 0;
        secondStreamTime       = 0;
        mathLoaderFirstStream  = 0;
        mathLoaderSecondStream = 0;
    }

    // Блок инициализации
    void initialization(const double& p11, const double& p22,
                        const int& stoplightState, const int& environmentState,
                        const std::vector<double>& goodTime, const std::vector<double>& badTime,
                        const double& lambda1Good, const double& lambda2Good,
                        const double& lambda1Bad, const double& lambda2Bad,
                        const int& kappa1, const int& kappa2,
                        const double& fi1Good, const double& fi2Good, const double& fi1Bad, const double& fi2Bad,
                        const double& p1, const double& q1, const double& p2, const double& q2) {
        environment.setCurrentState(environmentState);
        environment.setProbability(p11, p22);
        environment.setStationaryDistribution();

        stoplight.setCurrentState(stoplightState);
        stoplight.setTimeGoodWeather(goodTime);
        stoplight.setTimeBadWeather(badTime);

        stream1.setLambdaGoodWeather(lambda1Good);
        stream1.setLambdaBadWeather(lambda1Bad);
        stream1.setKappa(kappa1);
        if (kappa1 != 0) {
            stream1.setArrivedCarsStream(kappa1);
            std::queue<double> helpQueue1;
            for (int           i = 0; i < kappa1; i++) {
                helpQueue1.push(0);
            }
            stream1.setArriveTime(helpQueue1);
        }

        stream1.setFiGoodWeather(fi1Good);
        stream1.setFiBadWeather(fi1Bad);
        stream1.setKsiMaxGoodWeather(static_cast<int>(goodTime[0] * fi1Good));
        stream1.setKsiMaxBadWeather(static_cast<int>(badTime[0] * fi1Bad));
        stream1.setP(p1);
        stream1.setQ(q1);
        stream1.setS(1 - q1 - p1);

        stream2.setLambdaGoodWeather(lambda2Good);
        stream2.setLambdaBadWeather(lambda2Bad);
        stream2.setKappa(kappa2);
        if (kappa2 != 0) {
            stream2.setArrivedCarsStream(kappa2);
            std::queue<double> helpQueue2;
            for (int           i = 0; i < kappa2; i++) {
                helpQueue2.push(0);
            }
            stream2.setArriveTime(helpQueue2);
        }

        stream2.setFiGoodWeather(fi2Good);
        stream2.setFiBadWeather(fi2Bad);
        stream2.setKsiMaxGoodWeather(static_cast<int>(goodTime[2] * fi2Good));
        stream2.setKsiMaxBadWeather(static_cast<int>(badTime[2] * fi2Bad));
        stream2.setP(p2);
        stream2.setQ(q2);
        stream2.setS(1 - q2 - p2);
    }

    // Проверка наличия в системе стационарного режима [Методичка Е.В. Пройдаковой]
    void checkStationary() {
        isStationary = true;

        {
            double lhs = stream1.getLambdaGoodWeather() * stoplight.getCycleTimeGW();
            double rhs = stream1.getFiGoodWeather() * stoplight.getTimeGoodWeather().at(0);
            if (lhs > rhs) {
                isStationary = false;
            }
        }

        {
            double lhs = stream2.getLambdaGoodWeather() * stoplight.getCycleTimeGW();
            double rhs = stream2.getFiGoodWeather() * stoplight.getTimeGoodWeather().at(2);
            if (lhs > rhs) {
                isStationary = false;
            }
        }

        {
            double lhs = stream1.getLambdaBadWeather()
                         * (stream1.getP() * 1 + stream1.getQ() * 2 + stream1.getS() * 3) * stoplight.getCycleTimeBW();
            double rhs = stream1.getFiBadWeather() * stoplight.getTimeBadWeather().at(0);
            if (lhs > rhs) {
                isStationary = false;
            }
        }

        {
            double lhs = stream2.getLambdaBadWeather()
                         * (stream2.getP() * 1 + stream2.getQ() * 2 + stream2.getS() * 3) * stoplight.getCycleTimeBW();
            double rhs = stream2.getFiBadWeather() * stoplight.getTimeBadWeather().at(2);
            if (lhs > rhs) {
                isStationary = false;
            }
        }

        double sd1 = environment.getStationaryDistribution1();
        double sd2 = environment.getStationaryDistribution2();

        mathLoaderFirstStream = sd1 * stream1.getLambdaGoodWeather() * stoplight.getCycleTimeGW()
                                + sd2 * stream1.getLambdaBadWeather()
                                  * (stream1.getP() * 1 + stream1.getQ() * 2 + stream1.getS() * 3)
                                  * stoplight.getCycleTimeBW();

        mathLoaderFirstStream /= stream1.getFiGoodWeather() * stoplight.getTimeGoodWeather().at(0) * sd1
                                 + stream1.getFiBadWeather() * stoplight.getTimeBadWeather().at(0) * sd2;

        mathLoaderSecondStream = sd1 * stream2.getLambdaGoodWeather() * stoplight.getCycleTimeGW()
                                 + sd2 * stream2.getLambdaBadWeather()
                                   * (stream2.getP() * 1 + stream2.getQ() * 2 + stream2.getS() * 3)
                                   * stoplight.getCycleTimeBW();

        mathLoaderSecondStream /= stream2.getFiGoodWeather() * stoplight.getTimeGoodWeather().at(2) * sd1
                                  + stream2.getFiBadWeather() * stoplight.getTimeBadWeather().at(2) * sd2;
    }

    // Определение следующего события
    void nextState(const double& randomVariable1, const double& randomVariable2, const double& randomVariable3,
                   const double& randomVariable4, const double& randomVariable5) {

        // При неизвестном времени прихода следующей заявки по первому потоку - ищем его
        if (flag1) {
            stream1.generateNextTimeArrive(environment.getCurrentState(),
                                           randomVariable1, randomVariable2);
        }

        // При неизвестном времени прихода следующей заявки по второму потоку - ищем его
        if (flag2) {
            stream2.generateNextTimeArrive(environment.getCurrentState(),
                                           randomVariable4, randomVariable5);
        }

        // Определение текущего времени при окончании обслуживания заявки по 1 потоку
        if (flagOnce1) {
            // Окончание обслуживания становится текущем временем,
            // если оно меньше времени прихода заявок по обоим потокам
            if ((stream1.getServiceTime() < stream2.getLastTime())
                && ((stream1.getServiceTime() < stream1.getLastTime()))
                // И если оно успеевает попасть в промежуток обслужвания
                && (stream1.getServiceTime()
                    < allTime + stoplight.getFirstStreamGreenTime(environment.getCurrentState()))) {
                currentTime = stream1.getServiceTime();
                flagOnce1   = false;
            }
        }

        // Определение текущего времени при окончании обслуживания заявки по 2 потоку
        if (flagOnce2) {
            // Окончание обслуживания становится текущем временем, если оно меньше времени прихода заявок по обоим потокам
            if ((stream2.getServiceTime() < stream2.getLastTime())
                && ((stream2.getServiceTime() < stream1.getLastTime()))
                // И если оно успеевает попасть в промежуток обслужвания
                && (stream2.getServiceTime()
                    < allTime + stoplight.getSecondStreamGreenTime(environment.getCurrentState()))) {
                currentTime = stream2.getServiceTime();
                flagOnce2   = false;
            }
        }

        // При зеленом светофоре для первого потока
        if (stoplight.getCurrentState() == 1) {
            // При не пустой очереди
            if (stream1.getKappa() != 0) {
                //  Время обсулживания последней заявки нестрого меньше текущего времени
                if ((stream1.getServiceTime() <= currentTime) &&
                    // Число обслуженных машин строго меньше максимального количества
                    (stream1.getKsiReal(environment.getCurrentState())
                     < stream1.getKsiMax(environment.getCurrentState()))) {
                    carArrive(flag3, flagOnce1, flag1, flag2, flag4, environment,
                              stoplight, stream1, currentTime);
                    return;
                }
            }
        }

        // При зеленом светофоре для второго потока
        if (stoplight.getCurrentState() == 3) {
            // При не пустой очереди
            if (stream2.getKappa() != 0) {
                //  Время обсулживания последней заявки нестрого меньше текущего времени
                if ((stream2.getServiceTime() <= currentTime) &&
                    // Число обслуженных машин строго меньше максимального количества
                    (stream2.getKsiReal(environment.getCurrentState())
                     < stream2.getKsiMax(environment.getCurrentState()))) {
                    carArrive(flag4, flagOnce2, flag1, flag2, flag3, environment,
                              stoplight, stream2, currentTime);
                    return;
                }
            }
        }

        if (stream1.getLastTime() < stream2.getLastTime()) {
            flag2 = false;
            flag3 = false;
            flag4 = false;
            // Если время прихода последней заявки по первому потоку, меньше чем время работы цикла светорфора
            if (timeless(stream1.getLastTime(), environment.getCurrentState(),
                         stoplight, allTime)) {
                // Следующее событие - пришла заявка по первому потоку
                flag1 = true;
                environment.setNextState(environment.getCurrentState());
                stoplight.setNextState(stoplight.getCurrentState());
                currentTime = stream1.getLastTime();
            }
                // В ином случае меняем состояние среды.
            else {
                // Если зеленый свет для первого потока, т.е. прошёл один цикл
                if (stoplight.getCurrentState() == 1) {
                    updateData(stream1, stream2, averageQueue1, averageQueue2,
                               counter, sqrAverageQueue1, sqrAverageQueue2,
                               averageVariationQueue1, averageVariationQueue2);
                }
                flag1 = false;
                environment.calculateNextState(randomVariable5);
                stoplight.calculateNextState();
            }
        } else {
            flag1 = false;
            flag3 = false;
            flag4 = false;
            if (timeless(stream2.getLastTime(), environment.getCurrentState(),
                         stoplight, allTime)) {
                // Следующее событие - пришла заявка по второму потоку
                flag2 = true;
                environment.setNextState(environment.getCurrentState());
                stoplight.setNextState(stoplight.getCurrentState());
                currentTime = stream2.getLastTime();
            } else {
                // Если зеленый свет для первого потока, т.е. прошёл один цикл
                if (stoplight.getCurrentState() == 3) {
                    updateData(stream1, stream2, averageQueue1, averageQueue2,
                               counter, sqrAverageQueue1, sqrAverageQueue2,
                               averageVariationQueue1, averageVariationQueue2);
                }
                flag2 = false;
                environment.calculateNextState(randomVariable5);
                stoplight.calculateNextState();
            }
        }
    }

    // Смена состояния
    void changeState() {
        if (flag1) {
            updateOnArrive(stream1);
        } else if (flag2) {
            updateOnArrive(stream2);
        } else if (flag3) {
            updateOnLeave(stream1, averageWaitingTime1, currentTime,
                          sqrAverageTime1, averageVariationTime1, timeLastCar1);
        } else if (flag4) {
            updateOnLeave(stream2, averageWaitingTime2, currentTime,
                          sqrAverageTime2, averageVariationTime2, timeLastCar2);
        } else {
            if (environment.getCurrentState() == 1) {
                allTime += stoplight.getTimeGoodWeather().at(stoplight.getCurrentState() - 1);
                timeGoodWeather += stoplight.getTimeGoodWeather().at(stoplight.getCurrentState() - 1);

                if ((stoplight.getCurrentState() == 1) || (stoplight.getCurrentState() == 2)) {
                    firstStreamTime += stoplight.getTimeGoodWeather().at(stoplight.getCurrentState() - 1);
                } else {
                    secondStreamTime += stoplight.getTimeGoodWeather().at(stoplight.getCurrentState() - 1);
                }
            }

            if (environment.getCurrentState() == 2) {
                allTime += stoplight.getTimeBadWeather().at(stoplight.getCurrentState() - 1);
                timeBadWeather += stoplight.getTimeBadWeather().at(stoplight.getCurrentState() - 1);
                if ((stoplight.getCurrentState() == 1) || (stoplight.getCurrentState() == 2)) {
                    firstStreamTime += stoplight.getTimeBadWeather().at(stoplight.getCurrentState() - 1);
                } else {
                    secondStreamTime += stoplight.getTimeBadWeather().at(stoplight.getCurrentState() - 1);
                }
            }

            currentTime = allTime;
            stream1.setServiceTime(currentTime);
            stream2.setServiceTime(currentTime);
        }

        arrivedCars = stream1.getArrivedCarsStream() + stream2.getArrivedCarsStream();

        stoplight.changeState();

        environment.changeState();
    }

    [[nodiscard]] double getOneTimeCycle() const {
        if (stoplight.getCycleTimeGW() > stoplight.getCycleTimeBW()) {
            return stoplight.getCycleTimeGW();
        }
        return stoplight.getCycleTimeBW();
    }

    [[nodiscard]] double getStationaryDistribution1() const {
        return environment.getStationaryDistribution1();
    }

    [[nodiscard]] double getStationaryDistribution2() const {
        return environment.getStationaryDistribution2();
    }


    [[nodiscard]] double getLoader1() const {
        if (allTime != 0) {
            return stream1.getLoader() / allTime;
        }
        return 0;
    }

    [[nodiscard]] double getLoader2() const {
        if (allTime != 0) {
            return stream2.getLoader() / allTime;
        }
        return 0;
    }

    [[nodiscard]] double getMathLoaderFirstStream() const {
        return mathLoaderFirstStream;
    }

    [[nodiscard]] double getMathLoaderSecondStream() const {
        return mathLoaderSecondStream;
    }

    [[nodiscard]] double getStatLoaderFirstStream() const {
        if (firstStreamTime != 0) {
            return stream1.getLoader() / firstStreamTime;
        }
        return 0;
    }

    [[nodiscard]] double getStatLoaderSecondStream() const {
        if (secondStreamTime != 0) {
            return stream2.getLoader() / secondStreamTime;
        }
        return 0;
    }

    [[nodiscard]] double getLambda1_1() const {
        return stream1.getLambdaGoodWeather();
    }

    [[nodiscard]] double getLambda2_1() const {
        return stream2.getLambdaGoodWeather();
    }

    [[nodiscard]] double getLambda1_2() const {
        return stream1.getLambdaBadWeather();
    }

    [[nodiscard]] double getLambda2_2() const {
        return stream2.getLambdaBadWeather();
    }

    [[nodiscard]] double getP1() const {
        return stream1.getP();
    }

    [[nodiscard]] double getP2() const {
        return stream2.getP();
    }

    [[nodiscard]] double getQ1() const {
        return stream1.getQ();
    }

    [[nodiscard]] double getQ2() const {
        return stream2.getQ();
    }

    [[nodiscard]] double getS1() const {
        return stream1.getS();
    }

    [[nodiscard]] double getS2() const {
        return stream2.getS();
    }

    [[nodiscard]] bool getIsStationary() const {
        return isStationary;
    }

    [[nodiscard]] double getTimeLastCar1() const {
        return timeLastCar1;
    }

    [[nodiscard]] double getTimeLastCar2() const {
        return timeLastCar2;
    }

    [[nodiscard]] double getCurrentTime() const {
        return currentTime;
    }

    [[nodiscard]] double getAverageWaitingTime1() const {
        return averageWaitingTime1;
    }

    [[nodiscard]] double getAverageWaitingTime2() const {
        return averageWaitingTime2;
    }
};
