//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_VOBJECT_H_
#define DIGITALVOX_VOX_RENDER_VOBJECT_H_

#include <nlohmann/json.hpp>

namespace vox {
class VObject {
public:
    virtual ~VObject() = default;
    
    /**
     * Called when the serialization is asked
     */
    virtual void on_serialize(nlohmann::json &data) = 0;
    
    /**
     * Called when the deserialization is asked
     */
    virtual void on_deserialize(const nlohmann::json &data) = 0;
};

}

#endif /* DIGITALVOX_VOX_RENDER_VOBJECT_H_ */
