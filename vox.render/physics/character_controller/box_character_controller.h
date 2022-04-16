//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef box_character_controller_hpp
#define box_character_controller_hpp

#include "character_controller.h"

namespace vox {
namespace physics {
/**
 * A box character controller.
 */
class BoxCharacterController : public CharacterController {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;
    
    BoxCharacterController(Entity *entity);
    
    void setDesc(const PxBoxControllerDesc &desc);
    
    float halfHeight() const;
    
    float halfSideExtent() const;
    
    float halfForwardExtent() const;
    
    bool setHalfHeight(float halfHeight);
    
    bool setHalfSideExtent(float halfSideExtent);
    
    bool setHalfForwardExtent(float halfForwardExtent);
    
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

#endif /* box_character_controller_hpp */
