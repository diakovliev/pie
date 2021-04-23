#pragma once

#include <thread>
#include <chrono>

namespace piel::lib {

    template<class chrono_units = std::chrono::seconds>
    class SleepFor {
    public:
        using UnitsCount = unsigned int;

        SleepFor(UnitsCount tm): tm_(tm) {}
        SleepFor(const SleepFor&) = default;
        SleepFor(SleepFor&&) = default;

        void operator()() const {
            std::this_thread::sleep_for(chrono_units(tm_));
        }

    private:
        UnitsCount tm_;

    };

}//namespace piel::lib
