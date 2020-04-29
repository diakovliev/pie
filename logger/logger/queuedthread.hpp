#pragma once

#include <queue>
#include <memory>
#include <thread>
#include <optional>
#include <iostream>
#include <atomic>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace piel { namespace lib {

template <class  T, int wait_timeout_ms = 500>
class QueuedThread
{
public:
    typedef QueuedThread<T, wait_timeout_ms>    ThisType;
    typedef std::shared_ptr<ThisType>           ThisPtr;
    typedef std::queue<T>                       Queue;
    typedef std::shared_ptr<Queue>              QueuePtr;
    typedef std::shared_ptr<std::thread>        ThreadPtr;

    std::function<int (const T& m)>             on_message;

    virtual ~QueuedThread() {
        join();
    }

    inline static ThisPtr start() {
        ThisPtr instance(new ThisType());
        instance->thread_.reset(new std::thread(std::bind(&ThisType::entry, instance.get())));
        return instance;
    }

    void join() {
        if (thread_->joinable()) {
            thread_->join();
        }
    }

    void enqueue(T v) {
        if (quit_) return;
        std::unique_lock<std::mutex> lock{mutex_};
        queue_->push(v);
        cond_.notify_all();
    }

    void complete_and_join() {
        quit_ = true;
        join();
    }

    QueuedThread()
        : quit_(false)
        , queue_(new Queue())
        , mutex_()
        , cond_()
        , thread_()
    {
    }

protected:
    std::optional<T> dequeue() {
        std::unique_lock<std::mutex> lock{mutex_};
        std::optional<T> ret_val = std::nullopt;

        if (!queue_->empty())
        {
            ret_val = queue_->front();
            queue_->pop();
        }

        return ret_val;
    }

    void entry() {
        bool quit = false;

        while (!quit) {
            std::optional<T> opt = dequeue();
            if (opt) {
                int ret = on_message(*opt);
                if (ret < 0)
                {
                    quit_ = true;
                }
            } else {
                if (quit_) {
                    quit = true;
                    continue;
                }
                std::unique_lock<std::mutex> lock{mutex_};
                cond_.wait_for(lock, std::chrono::milliseconds(wait_timeout_ms));
            }
        }
    }

private:
    std::atomic_bool            quit_;
    QueuePtr                    queue_;
    std::mutex                  mutex_;
    std::condition_variable     cond_;
    ThreadPtr                   thread_;
};

} } // namespace piel::lib
