#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace piel { namespace lib { namespace logger_utils {

template <typename T>
class Singleton
{
public:
    static T* get_instance() {
        static T p_instance;// = new T();
        return &p_instance;
    }

protected:
    Singleton() {}
    ~Singleton() {}

};

} } } // namespace piel::lib::logger_utils
