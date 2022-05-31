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
#include "vox.force/id_factory.h"

namespace vox::force {
#define START_TIMING(timerName) Timing::StartTiming(timerName);

#define STOP_TIMING Timing::StopTiming(false);

#define STOP_TIMING_PRINT Timing::StopTiming(true);

#define STOP_TIMING_AVG                            \
    {                                              \
        static int timing_timerId = -1;            \
        Timing::StopTiming(false, timing_timerId); \
    }

#define STOP_TIMING_AVG_PRINT                     \
    {                                             \
        static int timing_timerId = -1;           \
        Timing::StopTiming(true, timing_timerId); \
    }

#define INIT_TIMING                                              \
    int IDFactory::id = 0;                                       \
    std::unordered_map<int, AverageTime> Timing::m_average_times; \
    std::stack<TimingHelper> Timing::m_timing_stack;              \
    bool Timing::m_dont_print_times = false;                       \
    unsigned int Timing::m_start_counter = 0;                     \
    unsigned int Timing::m_stop_counter = 0;

/** \brief Struct to store a time measurement.
 */
struct TimingHelper {
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    std::string name;
};

/** \brief Struct to store the total time and the number of steps in order to compute the average time.
 */
struct AverageTime {
    double total_time{};
    unsigned int counter{};
    std::string name;
};

/** \brief Class for time measurements.
 */
struct Timing {
public:
    static bool m_dont_print_times;
    static unsigned int m_start_counter;
    static unsigned int m_stop_counter;
    static std::stack<TimingHelper> m_timing_stack;
    static std::unordered_map<int, AverageTime> m_average_times;

    static void Reset() {
        while (!m_timing_stack.empty()) m_timing_stack.pop();
        m_average_times.clear();
        m_start_counter = 0;
        m_stop_counter = 0;
    }

    FORCE_INLINE static void StartTiming(const std::string &name = std::string("")) {
        TimingHelper h;
        h.start = std::chrono::high_resolution_clock::now();
        h.name = name;
        Timing::m_timing_stack.push(h);
        Timing::m_start_counter++;
    }

    FORCE_INLINE static double StopTiming(bool print = true) {
        if (!Timing::m_timing_stack.empty()) {
            Timing::m_stop_counter++;
            std::chrono::time_point<std::chrono::high_resolution_clock> stop =
                    std::chrono::high_resolution_clock::now();
            TimingHelper h = Timing::m_timing_stack.top();
            Timing::m_timing_stack.pop();
            std::chrono::duration<double> elapsed_seconds = stop - h.start;
            double t = elapsed_seconds.count() * 1000.0;

            if (print) LOGI("time {}: {} ms", h.name.c_str(), t)
            return t;
        }
        return 0;
    }

    FORCE_INLINE static double StopTiming(bool print, int &id) {
        if (id == -1) id = IDFactory::GetId();
        if (!Timing::m_timing_stack.empty()) {
            Timing::m_stop_counter++;
            std::chrono::time_point<std::chrono::high_resolution_clock> stop =
                    std::chrono::high_resolution_clock::now();
            TimingHelper h = Timing::m_timing_stack.top();
            Timing::m_timing_stack.pop();

            std::chrono::duration<double> elapsed_seconds = stop - h.start;
            double t = elapsed_seconds.count() * 1000.0;

            if (print && !Timing::m_dont_print_times) LOGI("time {}: {} ms", h.name.c_str(), t)

            if (id >= 0) {
                std::unordered_map<int, AverageTime>::iterator iter;
                iter = Timing::m_average_times.find(id);
                if (iter != Timing::m_average_times.end()) {
                    Timing::m_average_times[id].total_time += t;
                    Timing::m_average_times[id].counter++;
                } else {
                    AverageTime at;
                    at.counter = 1;
                    at.total_time = t;
                    at.name = h.name;
                    Timing::m_average_times[id] = at;
                }
            }
            return t;
        }
        return 0;
    }

    FORCE_INLINE static void PrintAverageTimes() {
        std::unordered_map<int, AverageTime>::iterator iter;
        for (iter = Timing::m_average_times.begin(); iter != Timing::m_average_times.end(); iter++) {
            AverageTime &at = iter->second;
            const double kAvgTime = at.total_time / at.counter;
            LOGI("Average time {} avgTime {}", at.name.c_str(), kAvgTime)
        }
        if (Timing::m_start_counter != Timing::m_stop_counter)
            LOGI("Problem: {} calls of StartTiming and {} calls of StopTiming.", Timing::m_start_counter,
                 Timing::m_stop_counter)
        LOGI("---------------------------------------------------------------------------\n")
    }

    FORCE_INLINE static void PrintTimeSums() {
        std::unordered_map<int, AverageTime>::iterator iter;
        for (iter = Timing::m_average_times.begin(); iter != Timing::m_average_times.end(); iter++) {
            AverageTime &at = iter->second;
            const double kTimeSum = at.total_time;
            LOGI("Time sum {} : timeSum {}", at.name.c_str(), kTimeSum)
        }
        if (Timing::m_start_counter != Timing::m_stop_counter)
            LOGI("Problem: {} calls of StartTiming and {} calls of StopTiming.", Timing::m_start_counter,
                 Timing::m_stop_counter)
        LOGI("---------------------------------------------------------------------------\n")
    }
};
}  // namespace vox::force
