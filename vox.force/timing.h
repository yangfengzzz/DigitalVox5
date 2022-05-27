//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <chrono>
#include <iostream>
#include <stack>
#include <unordered_map>

#include "vox.base/logging.h"

namespace vox::force {
#define START_TIMING(timerName) Utilities::Timing::startTiming(timerName);

#define STOP_TIMING Utilities::Timing::stopTiming(false);

#define STOP_TIMING_PRINT Utilities::Timing::stopTiming(true);

#define STOP_TIMING_AVG                                       \
    {                                                         \
        static int timing_timerId = -1;                       \
        Utilities::Timing::stopTiming(false, timing_timerId); \
    }

#define STOP_TIMING_AVG_PRINT                                \
    {                                                        \
        static int timing_timerId = -1;                      \
        Utilities::Timing::stopTiming(true, timing_timerId); \
    }

#define INIT_TIMING                                                                    \
    int Utilities::IDFactory::id = 0;                                                  \
    std::unordered_map<int, Utilities::AverageTime> Utilities::Timing::m_averageTimes; \
    std::stack<Utilities::TimingHelper> Utilities::Timing::m_timingStack;              \
    bool Utilities::Timing::m_dontPrintTimes = false;                                  \
    unsigned int Utilities::Timing::m_startCounter = 0;                                \
    unsigned int Utilities::Timing::m_stopCounter = 0;

/** \brief Struct to store a time measurement.
 */
struct TimingHelper {
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    std::string name;
};

/** \brief Struct to store the total time and the number of steps in order to compute the average time.
 */
struct AverageTime {
    double totalTime{};
    unsigned int counter{};
    std::string name;
};

/** \brief Factory for unique ids.
 */
class IDFactory {
private:
    /** Current id */
    static int id;

public:
    static int getId() { return id++; }
};

/** \brief Class for time measurements.
 */
class Timing {
public:
    static bool m_dontPrintTimes;
    static unsigned int m_startCounter;
    static unsigned int m_stopCounter;
    static std::stack<TimingHelper> m_timingStack;
    static std::unordered_map<int, AverageTime> m_averageTimes;

    static void reset() {
        while (!m_timingStack.empty()) m_timingStack.pop();
        m_averageTimes.clear();
        m_startCounter = 0;
        m_stopCounter = 0;
    }

    FORCE_INLINE static void startTiming(const std::string &name = std::string("")) {
        TimingHelper h;
        h.start = std::chrono::high_resolution_clock::now();
        h.name = name;
        Timing::m_timingStack.push(h);
        Timing::m_startCounter++;
    }

    FORCE_INLINE static double stopTiming(bool print = true) {
        if (!Timing::m_timingStack.empty()) {
            Timing::m_stopCounter++;
            std::chrono::time_point<std::chrono::high_resolution_clock> stop =
                    std::chrono::high_resolution_clock::now();
            TimingHelper h = Timing::m_timingStack.top();
            Timing::m_timingStack.pop();
            std::chrono::duration<double> elapsed_seconds = stop - h.start;
            double t = elapsed_seconds.count() * 1000.0;

            if (print) LOGI("time {}: {} ms", h.name.c_str(), t)
            return t;
        }
        return 0;
    }

    FORCE_INLINE static double stopTiming(bool print, int &id) {
        if (id == -1) id = IDFactory::getId();
        if (!Timing::m_timingStack.empty()) {
            Timing::m_stopCounter++;
            std::chrono::time_point<std::chrono::high_resolution_clock> stop =
                    std::chrono::high_resolution_clock::now();
            TimingHelper h = Timing::m_timingStack.top();
            Timing::m_timingStack.pop();

            std::chrono::duration<double> elapsed_seconds = stop - h.start;
            double t = elapsed_seconds.count() * 1000.0;

            if (print && !Timing::m_dontPrintTimes) LOGI("time {}: {} ms", h.name.c_str(), t)

            if (id >= 0) {
                std::unordered_map<int, AverageTime>::iterator iter;
                iter = Timing::m_averageTimes.find(id);
                if (iter != Timing::m_averageTimes.end()) {
                    Timing::m_averageTimes[id].totalTime += t;
                    Timing::m_averageTimes[id].counter++;
                } else {
                    AverageTime at;
                    at.counter = 1;
                    at.totalTime = t;
                    at.name = h.name;
                    Timing::m_averageTimes[id] = at;
                }
            }
            return t;
        }
        return 0;
    }

    FORCE_INLINE static void printAverageTimes() {
        std::unordered_map<int, AverageTime>::iterator iter;
        for (iter = Timing::m_averageTimes.begin(); iter != Timing::m_averageTimes.end(); iter++) {
            AverageTime &at = iter->second;
            const double avgTime = at.totalTime / at.counter;
            LOGI("Average time {} avgTime {}", at.name.c_str(), avgTime)
        }
        if (Timing::m_startCounter != Timing::m_stopCounter)
            LOGI("Problem: {} calls of startTiming and {} calls of stopTiming.", Timing::m_startCounter,
                 Timing::m_stopCounter)
        LOGI("---------------------------------------------------------------------------\n")
    }

    FORCE_INLINE static void printTimeSums() {
        std::unordered_map<int, AverageTime>::iterator iter;
        for (iter = Timing::m_averageTimes.begin(); iter != Timing::m_averageTimes.end(); iter++) {
            AverageTime &at = iter->second;
            const double timeSum = at.totalTime;
            LOGI("Time sum {} : timeSum {}", at.name.c_str(), timeSum)
        }
        if (Timing::m_startCounter != Timing::m_stopCounter)
            LOGI("Problem: {} calls of startTiming and {} calls of stopTiming.", Timing::m_startCounter,
                 Timing::m_stopCounter)
        LOGI("---------------------------------------------------------------------------\n")
    }
};
}  // namespace vox::force
