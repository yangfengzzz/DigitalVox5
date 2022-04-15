//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_SINGLETON_H_
#define DIGITALVOX_VOX_RENDER_SINGLETON_H_

#include <cassert>

namespace vox {
/*
 * Template class for creating single-instance global classes.
 */
template<typename T>
class Singleton {
public:
    /** @brief Explicit private copy constructor. This is a forbidden operation.*/
    Singleton(const Singleton<T> &) = delete;
    
    /** @brief Private operator= . This is a forbidden operation. */
    Singleton &operator=(const Singleton<T> &) = delete;
    
protected:
    
    static T *ms_singleton_;
    
public:
    Singleton() {
        assert(!ms_singleton_);
        ms_singleton_ = static_cast< T * >( this );
    }
    
    ~Singleton() {
        assert(ms_singleton_);
        ms_singleton_ = 0;
    }
    
    static T &get_singleton() {
        assert(ms_singleton_);
        return (*ms_singleton_);
    }
    
    static T *get_singleton_ptr() {
        return ms_singleton_;
    }
};

}
#endif /* DIGITALVOX_VOX_RENDER_SINGLETON_H_ */
