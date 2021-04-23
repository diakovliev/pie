#pragma once

#include <functional>

namespace piel::lib {

    template<class Sleeper>
    class Retrier {
    public:
        using Counter   = unsigned int;
        using Target    = std::function<bool(Counter, Counter)>;

        Retrier(Target target, Counter max_attempts, Sleeper sleeper)
            : target_(target)
            , max_attempts_(max_attempts)
            , sleeper_(sleeper)
        {
        }

        void operator()() {
            Counter counter = 0;
            while(counter < max_attempts_ || max_attempts_ == 0) {
                if (counter > 0) {
                    sleeper_();
                }

                try {
                    if (target_(counter + 1, max_attempts_)) {
                        break;
                    }
                } catch (...) {
                    if (counter + 1 >= max_attempts_) {
                        // This is a last attempt, so rethrow an exception.
                        throw;
                    }
                }

                ++counter;
            }
        }

    private:
        Retrier() = delete;
        Retrier(const Retrier&) = delete;
        Retrier(Retrier&&) = delete;
        Retrier operator=(const Retrier&) = delete;

        Target      target_;
        Counter     max_attempts_;
        Sleeper     sleeper_;
    };

}//namespace piel::lib
