#include <chrono>
#include <string>
#include <iostream>

const inline bool REPORT_TIMINGS = false;

class ScopedTimer {
public:
    ScopedTimer(const std::string& name)
        : name(name), start(std::chrono::high_resolution_clock::now()) {}

    ~ScopedTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto dur = std::chrono::duration<double, std::milli>(end - start).count();
        if(REPORT_TIMINGS == true)
            std::cout << "[TIMER] " << name << ": " << dur << " ms" << std::endl;
    }

private:
    std::string name;
    std::chrono::high_resolution_clock::time_point start;
};

