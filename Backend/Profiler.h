#pragma once

#include <chrono>
#include <iostream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

using namespace std::chrono;

class LogDuration {
public:
    explicit LogDuration(const std::string& msg = "")
            : message(msg + ": "), start(steady_clock::now()) {}

    ~LogDuration() {
        std::ofstream output("time.txt");

        auto finish = steady_clock::now();
        auto dur    = finish - start;

        output << message
               << duration_cast<milliseconds>(dur).count()
               << " ms" << std::endl;
    }

private:
    steady_clock::time_point start;
    std::string              message;
};

#define UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
#define UNIQ_ID(lineno) UNIQ_ID_IMPL(lineno)

#define LOG_DURATION(message) \
  LogDuration UNIQ_ID(__LINE__){message};
