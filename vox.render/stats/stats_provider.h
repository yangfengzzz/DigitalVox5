//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "stats_common.h"

#include <map>
#include <set>
#include <unordered_map>

namespace vox {
class CommandBuffer;

/**
 * @brief Abstract interface for all StatsProvider classes
 */
class StatsProvider {
public:
    struct Counter {
        double result;
    };
    
    using Counters = std::unordered_map<StatIndex, Counter, StatIndexHash>;
    
    /**
     * @brief Virtual Destructor
     */
    virtual ~StatsProvider() = default;
    
    /**
     * @brief Checks if this provider can supply the given enabled stat
     * @param index The stat index
     * @return True if the stat is available, false otherwise
     */
    [[nodiscard]] virtual bool is_available(StatIndex index) const = 0;
    
    /**
     * @brief Retrieve graphing data for the given enabled stat
     * @param index The stat index
     */
    [[nodiscard]] virtual const StatGraphData &get_graph_data(StatIndex index) const {
        return default_graph_map_.at(index);
    }
    
    /**
     * @brief Retrieve default graphing data for the given stat
     * @param index The stat index
     */
    static const StatGraphData &default_graph_data(StatIndex index);
    
    /**
     * @brief Retrieve a new sample set
     * @param delta_time Time since last sample
     */
    virtual Counters sample(float delta_time) = 0;
    
    /**
     * @brief Retrieve a new sample set from continuous sampling
     * @param delta_time Time since last sample
     */
    virtual Counters continuous_sample(float delta_time) {
        return {};
    }
    
    /**
     * @brief A command buffer that we want stats about has just begun
     * @param cb The command buffer
     */
    virtual void begin_sampling(CommandBuffer &cb) {}
    
    /**
     * @brief A command buffer that we want stats about is about to be ended
     * @param cb The command buffer
     */
    virtual void end_sampling(CommandBuffer &cb) {}
    
protected:
    static std::map<StatIndex, StatGraphData> default_graph_map_;
};

}        // namespace vox
