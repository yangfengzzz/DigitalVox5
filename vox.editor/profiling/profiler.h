//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "profiling/profiler_report.h"
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <thread>

namespace vox {
/* Forward declaration of the profiler spy structure */
struct ProfilerSpy;

/**
 * The profiler collect data about the running program
 */
class Profiler final {
public:
    /**
     * Create the profiler
     */
    Profiler();
    
    /**
     * Generate a report containing data about the last profiling session
     */
    ProfilerReport generate_report();
    
    /**
     * Clear any collected data
     */
    void clear_history();
    
    /**
     * Update the profiler
     */
    static void update(float delta_time);
    
    /**
     * Save the given spy collected data to the profiler history
     * @param spy (Spy to collect data from)
     */
    static void save(ProfilerSpy &spy);
    
    /**
     * Verify if the profiler is currently enabled
     */
    static bool is_enabled();
    
    /**
     * Enable the profiler if it is currently disabled, and vice-versa
     */
    static void toggle_enable();
    
    /**
     * Enable the profiler
     */
    static void enable();
    
    /**
     * Disable the profiler
     */
    static void disable();
    
private:
    /* Time relatives */
    std::chrono::steady_clock::time_point last_time_;
    
    /* Profiler settings */
    static bool enabled_;
    
    /* Collected data */
    static std::mutex save_mutex_;
    static std::unordered_map<std::string, double> elapsed_history_;
    static std::unordered_map<std::string, uint64_t> calls_counter_;
    static std::vector<std::thread::id> working_threads_;
    static uint32_t elapsed_frames_;
};

}
