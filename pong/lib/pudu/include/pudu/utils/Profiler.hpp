#pragma once

#include <cstdint>

namespace pudu
{

class Profiler
{
public:
    Profiler() = default;
    ~Profiler() = default;

    void init();
    void startMeasurement();
    void stopMeasurement();

    inline double getElapsedTime() const { return m_elapsedTime; }

private:
    uint32_t m_startQueryId = 0;
    uint32_t m_endQueryId = 0;
    uint64_t m_startTime = 0;
    uint64_t m_stopTime = 0;

    double m_elapsedTime = 0.0;
};

}