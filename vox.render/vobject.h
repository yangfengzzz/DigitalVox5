//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef vobject_hpp
#define vobject_hpp

#include <json.hpp>

namespace vox {
class VObject {
public:
    virtual ~VObject() {}
    
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

#endif /* vobject_hpp */
