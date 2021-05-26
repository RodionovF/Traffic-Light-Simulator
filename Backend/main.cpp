#include <cstdlib>
//#include <future>
#include <algorithm>
#include <map>
//#include <ctime>
//#include <future>
#include <omp.h>
#include <memory>
#include <random>
//#include <set>

#include "Condition.h"
#include "Profiler.h"

using namespace std;

// Работа программы
void init(std::shared_ptr<Condition> condition, std::shared_ptr<Condition> conditionLoading, int& counter,
          bool& initialFlag, bool& can, const bool& optimize_flag,
          const double& T1_1, const double& T2_1, const double& T1_3,
          const double& T2_3) {
    can     = true;
    counter = 0;

    std::vector<double> goodTime; // Время работы светофора при хорошем состоянии внешней среды
    std::vector<double> badTime; // Время работы светофора при плохом состоянии внешней среды

    if (!optimize_flag) {
        goodTime.emplace_back(4);
        goodTime.emplace_back(17);
        goodTime.emplace_back(4);
        goodTime.emplace_back(18);
    } else {
        goodTime.emplace_back(T1_1);
        goodTime.emplace_back(4);
        goodTime.emplace_back(T1_3);
        goodTime.emplace_back(4);
    }

    if (!optimize_flag) {
        badTime.emplace_back(19);
        badTime.emplace_back(4);
        badTime.emplace_back(20);
        badTime.emplace_back(4);
    } else {
        badTime.emplace_back(T2_1);
        badTime.emplace_back(4);
        badTime.emplace_back(T2_3);
        badTime.emplace_back(4);
    }

    // std::cout << "First" << std::endl;

    conditionLoading->initialization(0.95, 0.8, 1, 1, goodTime, badTime, 0.075,
                                     0.065, 0.045, 0.055, 10, 10, 0.25, 0.25, 0.25, 0.25, 0.65, 0.25,
                                     0.6, 0.2);

    // std::cout << "Second" << std::endl;

    condition->initialization(0.95, 0.8, 1, 1, goodTime, badTime, 0.075, 0.065,
                              0.045, 0.055, 0, 0, 0.25, 0.25, 0.25, 0.25, 0.65, 0.25, 0.6, 0.2);
    // std::cout << "Third" << std::endl;
}

void calculate(std::shared_ptr<Condition> condition, std::shared_ptr<Condition> conditionLoading, int& counter,
               bool& initialFlag, bool& can) {
    //auto now_time = steady_clock::now();
    //auto t = duration_cast<milliseconds>(now_time).count();
    long long x;
    asm("rdtsc" : "=A"(x));
    srand(x);
    initialFlag = true;
    counter += 1500;

    bool flag               = true;
    bool flag1              = false;
    bool flag2              = false;
    int  ppConditionCounter = 0;
    while (flag) {

//		random_device rnd;
//		mt19937 gen(rnd());
//		uniform_real_distribution<> dist(0.,1.);
//
//		vector<double> data(5);
//		generate(data.begin(), data.end(), [&gen, &dist]() { return dist(gen); });

        double c1 = static_cast<double>(rand()) / RAND_MAX;
        double c2 = static_cast<double>(rand()) / RAND_MAX;
        double c3 = static_cast<double>(rand()) / RAND_MAX;
        double c4 = static_cast<double>(rand()) / RAND_MAX;
        double c5 = static_cast<double>(rand()) / RAND_MAX;

        if (condition->getCurrentTime()
            >= condition->getOneTimeCycle() * counter) {
            flag1 = true;
        } else {
            condition->nextState(c1, c2, c3, c4, c5);
            condition->changeState();
        }

        if (conditionLoading->getCurrentTime()
            >= conditionLoading->getOneTimeCycle() * counter) {
            flag2 = true;
        } else {
            conditionLoading->nextState(c1, c2, c3, c4, c5);
            conditionLoading->changeState();
        }

        if (flag1 && flag2) {
            flag = false;
        }

        double moGamma0_1    = condition->getAverageWaitingTime1();
        double moGamma0_2    = condition->getAverageWaitingTime2();
        double moGammaPlus_1 = conditionLoading->getAverageWaitingTime1();
        double moGammaPlus_2 = conditionLoading->getAverageWaitingTime2();

        if (can) {
            if ((abs(moGamma0_1 - moGammaPlus_1) < (0.05 * moGamma0_1))
                && (abs(moGamma0_2 - moGammaPlus_2) < (0.05 * moGamma0_2))) {
                ppConditionCounter++;
                if (ppConditionCounter == 10) {
                    can  = false;
                    flag = false;
                }
            } else {
                ppConditionCounter = 0;
            }
        }
    }
}

double getFunction(std::shared_ptr<Condition> condition, const double& tau1,
                   const double& tau2) {
    double lam1_1 = condition->getLambda1_1();
    double lam1_2 = condition->getLambda1_2();
    double lam2_1 = condition->getLambda2_1();
    double lam2_2 = condition->getLambda2_2();

    double p1 = condition->getP1();
    double p2 = condition->getP2();
    double q1 = condition->getQ1();
    double q2 = condition->getQ2();
    double s1 = condition->getS1();
    double s2 = condition->getS2();

    double a1 = lam1_1 * condition->getQ1()
                + lam1_2 * (p1 * 1 + q1 * 2 + s1 * 3) * condition->getQ2();
    double a2 = lam2_1 * condition->getQ1()
                + lam2_2 * (p2 * 1 + q2 * 2 + s2 * 3) * condition->getQ2();
    double d  = a1 + a2;

    return (tau1 * a1 + tau2 * a2) / d;
}

void repeat1(const double T1, const double T2, const double T3, const double T4,
             vector<pair<double, vector<double>>>& tmp) {

    std::shared_ptr<Condition> condition        = std::make_shared<Condition>();
    std::shared_ptr<Condition> conditionLoading = std::make_shared<Condition>();

    // std::cout << ("Lol1") << std::endl;
    int  counter     = 0;
    bool initialFlag = true, can = true;

    init(condition, conditionLoading, counter, initialFlag, can, true, T1, T2, T3, T4);
    // std::cout << ("Lol2") << std::endl;
    condition->checkStationary();
    // std::cout << ("Lol3") << std::endl;
    double ml1 = condition->getMathLoaderFirstStream();
    // std::cout << ("Lol4") << std::endl;
    double ml2 = condition->getMathLoaderSecondStream();
    // std::cout << ("Lol5") << std::endl;
    if (condition->getIsStationary()) {
        int       j       = 0;
        const int MAX_NUM = 500;
        double    M1      = 0;
        double    M2      = 0;
        while (j < MAX_NUM) {
            condition        = std::make_shared<Condition>();
            conditionLoading = std::make_shared<Condition>();
            init(condition, conditionLoading, counter, initialFlag, can, true, T1, T2, T3, T4);
            calculate(condition, conditionLoading, counter, initialFlag, can);

            double tau1 = condition->getTimeLastCar1();
            double tau2 = condition->getTimeLastCar2();

            M1 += tau1 / static_cast<double>(MAX_NUM);
            M2 += tau2 / static_cast<double>(MAX_NUM);
            //std::cout << j << std::endl;
            j++;
        }

        double load = condition->getLoader1() + condition->getLoader2();

        double sl1 = condition->getStatLoaderFirstStream();
        double sl2 = condition->getStatLoaderSecondStream();

        double average_value = getFunction(condition, M1, M2);

        vector<double> temporary = {T1, T2, T3, T4, M1, M2, load, ml1, sl1, ml2, sl2};

        //const int vsize = 16;
        const int vsize = 3;

        std::cout << (T1 - 15) * vsize * vsize * vsize + (T2 - 15) * vsize * vsize + (T3 - 15) * vsize + (T4 - 15)
                  << std::endl;
        tmp[(T1 - 15) * vsize * vsize * vsize + (T2 - 15) * vsize * vsize + (T3 - 15) * vsize + (T4 - 15)] = make_pair(
                average_value, temporary);
    } else {
        //const int vsize = 16;
        const int vsize = 3;
        std::cout << (T1 - 15) * vsize * vsize * vsize + (T2 - 15) * vsize * vsize + (T3 - 15) * vsize + (T4 - 15)
                  << std::endl;
        std::vector<double> temporary = {T1, T2, T3, T4, -1, -1, -1, ml1, -1, ml2, -1};
        tmp[(T1 - 15) * vsize * vsize * vsize + (T2 - 15) * vsize * vsize + (T3 - 15) * vsize + (T4 - 15)] = make_pair(
                -1, temporary);
    }
}

vector<pair<double, vector<double>>> optimize() {
    LOG_DURATION("Optimize");
    //vector<future<Stats>> futures;

    vector<pair<double, vector<double>>> res;
//    res.resize(66536); // 16 ^ 4
//    const int END = 31;
    res.resize(81);
    const int END = 18;
//#pragma omp parallel for //for num_threads(16) //collapse(4) //shared(averageTau)
    for (int  i   = 15; i < END; ++i) {
        for (int j = 15; j < END; ++j) {
            for (int k = 15; k < END; ++k) {
#pragma omp parallel for
                for (int z = 15; z < END; ++z) {
                    std::cout << i << j << k << z << std::endl;
                    repeat1(i, j, k, z, res);
                }
            }
        }
        //printf("i = %d, j= %d, k= %d, z= %d, threadId = %d \n", i, j, k, z, omp_get_thread_num());
    }
    sort(res.begin(), res.end());
    return res;
}

void printfile(const std::shared_ptr<Condition>& condition, const vector<pair<double, vector<double>>>& averageTau) {
    std::ofstream csv("output.csv");
    std::ofstream output("output.txt");
    output << "First stream's lambda for good weather = "
           << condition->getLambda1_1() << std::endl;
    output << "First stream's lambda for bad weather = "
           << condition->getLambda1_2() << std::endl;
    output << "Probability for a group of one car = " << condition->getP1()
           << std::endl;
    output << "Probability for a group of two car = " << condition->getQ1()
           << std::endl;
    output << "Probability for a group of three car = " << condition->getS1()
           << std::endl;
    output << std::endl;
    output << "Second stream's lambda for good weather = "
           << condition->getLambda2_1() << std::endl;
    output << "Second stream's lambda for bad weather = "
           << condition->getLambda2_2() << std::endl;
    output << "Probability for a group of one car = " << condition->getP2()
           << std::endl;
    output << "Probability for a group of two car = " << condition->getQ2()
           << std::endl;
    output << "Probability for a group of three car = " << condition->getS2()
           << std::endl;
    output << std::endl;
    output << "Stationary distribution: " << std::endl;
    output << "Good weather = " << condition->getStationaryDistribution1()
           << std::endl;
    output << "Bad weather = " << condition->getStationaryDistribution2();
    output << std::endl;
    output << std::endl;

    output << std::fixed << std::setprecision(5);
    csv << "Function,T_1_1,T_1_3,T_2_1,T_2_3,M1,M2,L,ML1,SL1,ML2,SL2" << std::endl;
    for (const auto& item : averageTau) {
        csv << item.first << "," << item.second.at(0) << ","
            << item.second.at(2) << "," << item.second.at(1) << ","
            << item.second.at(3) << "," << item.second.at(4) << ","
            << item.second.at(5) << "," << item.second.at(6) << ","
            << item.second.at(7) << "," << item.second.at(8) << ","
            << item.second.at(9) << "," << item.second.at(10);
        csv << std::endl;
    }
    csv << std::endl;
}

int main() {
    std::shared_ptr<Condition> condition        = std::make_shared<Condition>();
    std::shared_ptr<Condition> conditionLoading = std::make_shared<Condition>();


    bool initialFlag = false;
    bool can         = true;
    int  counter     = 0;

    vector<pair<double, vector<double>>> answer = optimize();
    // std::cout << "End optimize" << std::endl;
    init(condition, conditionLoading, counter, initialFlag, can, false, 0, 0, 0, 0);
    std::cout << "Print file";
    printfile(condition, answer);
    // std::cout << "The end.";
    return 0;
}

//int main()
//{
//    int id;
//#pragma omp parallel private(id)
//    {
//        id = omp_get_thread_num();
//        printf("%d: Hello World!\n", id);
//    }
//    return 0;
//}