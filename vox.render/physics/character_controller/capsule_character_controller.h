//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef capsule_character_controller_hpp
#define capsule_character_controller_hpp

#include "character_controller.h"

namespace vox {
namespace physics {
/**
 * A capsule character controller.
 */
class CapsuleCharacterController : public CharacterController {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;
    
    CapsuleCharacterController(Entity *entity);
    
    void setDesc(const PxCapsuleControllerDesc &desc);
    
    float radius() const;
    
    bool setRadius(float radius);
    
    float height() const;
    
    bool setHeight(float height);
    
    PxCapsuleClimbingMode::Enum climbingMode() const;
    
    bool setClimbingMode(PxCapsuleClimbingMode::Enum mode);
    
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
#endif /* capsule_character_controller_hpp */
