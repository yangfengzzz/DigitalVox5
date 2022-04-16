//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef static_collider_hpp
#define static_collider_hpp

#include "collider.h"

namespace vox {
namespace physics {
class StaticCollider : public Collider {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;
    
    StaticCollider(Entity *entity);
    
public:
    /**
     * Called when the serialization is asked
     */
    void on_serialize(nlohmann::json &data) override;
    
    /**
     * Called when the deserialization is asked
     */
    void on_deserialize(const nlohmann::json &data) override;
    
    /**
     * Defines how the component should be drawn in the inspector
     */
    void on_inspector(ui::WidgetContainer &p_root) override;
};

}
}

#endif /* static_collider_hpp */
