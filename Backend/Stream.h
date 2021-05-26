#pragma once

#include <queue>
#include <cmath>

class Stream {
    double lambdaGoodWeather; // Параметр интенсивности входного потока при хорошем состоянии внешней среды
    double lambdaBadWeather; // Параметр интенсивности входного потока при плохом состоянии внешней среды
    double p;  // Вероятность прихода в пачке одной машины
    double q;  // Вероятность прихода в пачке двух машин
    double s;  // Вероятность прихода в пачке трех машин
    double fiGoodWeather; // Интенсивность пропускной способности перекрестка при хорошем состоянии внешней среды
    double fiBadWeather; // Интенсивность пропускной способности перекреста при плохом состоянии внешней среды
    int    kappa;  // Число машин в очереди по потоку
    int    ksiMaxGoodWeather; // Максимальная пропускная способность перекреста при хорошем состоянии внешней среды
    int    ksiMaxBadWeather; //  Максимальная пропускная способность перекреста при плохом состоянии внешней среды
    int    ksiRealGoodWeather; // Реальное число обслуженных машин за время работы перекрестка при хорошем состоянии внешней среды
    int    ksiRealBadWeather; // Реальное число обслуженных машин за время работы перекрестка при плохом состоянии внешней среды
    int    eta;  // Число машин, пришедших на перекресток

    std::queue<double> arriveTime;  // Время прихода каждой заявки

    double lastTime;  // Время последней пришедшей заявки
    int    arrivedCarsStream;  // Пришло всего машин по потоку
    int    servicedCarsStream;  // Обслужено всего машин по потоку

    int    arrivedCarsForOneIteration;  // Число пришедших машин за цикл
    double serviceTime;  // Время обслуживания
    double loader;  // Загрузка системы

    int numberOf1;  // Число раз, когда пришла 1 машина
    int numberOf2;  // Число раз, когда пришли 2 машины
    int numberOf3;  // Число раз, когда пришли 3 машины

public:
    Stream() {
        lambdaGoodWeather          = 0.25;
        lambdaBadWeather           = 0.25;
        p                          = 0.5;
        q                          = 0.3;
        s                          = 0.2;
        fiGoodWeather              = 0.25;
        fiBadWeather               = 0.25;
        kappa                      = 0;
        ksiMaxBadWeather           = 7;
        ksiMaxGoodWeather          = 7;
        ksiRealGoodWeather         = 0;
        ksiRealBadWeather          = 0;
        eta                        = 0;
        lastTime                   = 0;
        arrivedCarsStream          = 0;
        servicedCarsStream         = 0;
        arrivedCarsForOneIteration = 0;
        serviceTime                = 0;
        loader                     = 0;
        numberOf1                  = 0;
        numberOf2                  = 0;
        numberOf3                  = 0;
    }

    void generateService(const int& environment) {
        if (environment == 1) {
            serviceTime += pow(fiGoodWeather, -1);
            loader += pow(fiGoodWeather, -1);
        } else if (environment == 2) {
            serviceTime += pow(fiBadWeather, -1);
            loader += pow(fiBadWeather, -1);
        }
    }

    void generateNextTimeArrive(const int& environment, const double& randomValue1, const double& randomValue2) {
        double timeNextArrive = 0;
        arrivedCarsForOneIteration = 0;
        if (environment == 1) {
            timeNextArrive = -log(randomValue1) / lambdaGoodWeather; // Время прихода i-ой машины
            lastTime += timeNextArrive;
            arriveTime.push(lastTime);  // Сохраняем время прихода i-ой машины
            ++arrivedCarsForOneIteration;  // Увеличиваем число пришедших машин
        } else if (environment == 2) {
            timeNextArrive = -log(randomValue1) / lambdaBadWeather; // Время прихода i-ой пачки машин машины
            double n = 0;
            if ((0 <= randomValue2) && (randomValue2 < p)) {
                n = 1;
                numberOf1++;
            } else if ((p <= randomValue2) && (randomValue2 < (p + q))) {
                n = 2;
                numberOf2++;
            } else if (((p + q) <= randomValue2) && (randomValue2 < (p + q + s))) {
                n = 3;
                numberOf3++;
            }
            lastTime += timeNextArrive;
            for (int i = 0; i < n; ++i) {
                arriveTime.push(lastTime); // Сохраняем время прихода машин в i-ой пачке
                ++arrivedCarsForOneIteration; // Запоминаем число пришедших машин
            }
        }
    }

    void setLambdaGoodWeather(const double& lambdaGood) {
        lambdaGoodWeather = lambdaGood;
    }

    [[nodiscard]] double getLambdaGoodWeather() const {
        return lambdaGoodWeather;
    }

    void setLambdaBadWeather(const double& lambdaBad) {
        lambdaBadWeather = lambdaBad;
    }

    [[nodiscard]] double getLambdaBadWeather() const {
        return lambdaBadWeather;
    }

    void setP(const double& _p) {
        p = _p;
    }

    [[nodiscard]] double getP() const {
        return p;
    }

    void setQ(const double& _q) {
        q = _q;
    }

    [[nodiscard]] double getQ() const {
        return q;
    }

    void setS(const double& _s) {
        s = _s;
    }

    [[nodiscard]] double getS() const {
        return s;
    }

    void setFiGoodWeather(const double& fiGood) {
        fiGoodWeather = fiGood;
    }

    [[nodiscard]] double getFiGoodWeather() const {
        return fiGoodWeather;
    }

    void setFiBadWeather(const double& fiBad) {
        fiBadWeather = fiBad;
    }

    [[nodiscard]] double getFiBadWeather() const {
        return fiBadWeather;
    }

    void setKappa(const int& _kappa) {
        kappa = _kappa;
    }

    [[nodiscard]] int getKappa() const {
        return kappa;
    }

    void setKsiMaxGoodWeather(const int& ksiMaxGood) {
        ksiMaxGoodWeather = ksiMaxGood;
    }

    void setKsiMaxBadWeather(const int& ksiMaxBad) {
        ksiMaxBadWeather = ksiMaxBad;
    }

    void setArriveTime(const std::queue<double>& arrive) {
        arriveTime = arrive;
    }

    [[nodiscard]] std::queue<double> getArriveTime() const {
        return arriveTime;
    }

    [[nodiscard]] double getLastTime() const {
        return lastTime;
    }

    void setArrivedCarsStream(const int& arrivedCars) {
        arrivedCarsStream = arrivedCars;
    }

    [[nodiscard]] int getArrivedCarsStream() const {
        return arrivedCarsStream;
    }

    void setServicedCarsStream(const int& servicedCars) {
        servicedCarsStream = servicedCars;
    }

    [[nodiscard]] int getServicedCarsStream() const {
        return servicedCarsStream;
    }

    [[nodiscard]] int getArrivedCarsForOneIteration() const {
        return arrivedCarsForOneIteration;
    }

    [[nodiscard]] double getServiceTime() const {
        return serviceTime;
    }

    void setServiceTime(const double& time) {
        serviceTime = time;
    }

    [[nodiscard]] int getKsiReal(const int& environment) const {
        if (environment == 1) {
            return ksiRealGoodWeather;
        } else {
            return ksiRealBadWeather;
        }
    }

    [[nodiscard]] int getKsiMax(const int& environment) const {
        if (environment == 1) {
            return ksiMaxGoodWeather;
        } else {
            return ksiMaxBadWeather;
        }
    }

    [[nodiscard]] double getLoader() const {
        return loader;
    }

    [[nodiscard]] int gerNumberOf1() const {
        return numberOf1;
    }

    [[nodiscard]] int gerNumberOf2() const {
        return numberOf2;
    }

    [[nodiscard]] int gerNumberOf3() const {
        return numberOf3;
    }
};
