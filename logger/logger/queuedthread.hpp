#pragma once

#include <queue>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/optional.hpp>
#include <boost/signals2.hpp>
#include <iostream>

namespace piel { namespace lib {

template <class  T, int wait_timeout_ms = 500>
class QueuedThread
{
public:
    typedef QueuedThread<T, wait_timeout_ms>    ThisType;
    typedef boost::shared_ptr<ThisType>         ThisPtr;
    typedef std::queue<T>                       Queue;
    typedef boost::shared_ptr<Queue>            QueuePtr;
    typedef boost::shared_ptr<boost::thread>    ThreadPtr;

    boost::signals2::signal<int (const T& m)>   on_message;

    virtual ~QueuedThread() {
        join();
    }

    inline static ThisPtr start() {
        ThisPtr instance(new ThisType());
        instance->thread_.reset(new boost::thread(boost::lambda::bind(&ThisType::entry, instance.get())));
        return instance;
    }

    void join() {
        thread_->join();
    }

    void enqueue(T v) {
        if (quit_) return;
        boost::unique_lock<boost::mutex> lock{mutex_};
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
    boost::optional<T> dequeue() {
        boost::unique_lock<boost::mutex> lock{mutex_};
        boost::optional<T> ret_val = boost::none;

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
            boost::optional<T> opt = dequeue();
            if (opt) {
                boost::optional<int> ret_opt = on_message(*opt);
                if (!ret_opt || (*ret_opt < 0))
                {
                    quit_ = true;
                }
            } else {
                if (quit_) {
                    quit = true;
                    continue;
                }
                boost::unique_lock<boost::mutex> lock{mutex_};
                cond_.timed_wait(lock, boost::get_system_time()+boost::posix_time::milliseconds(wait_timeout_ms));
            }
        }
    }

private:
    bool                        quit_;
    QueuePtr                    queue_;
    boost::mutex                mutex_;
    boost::condition_variable   cond_;
    ThreadPtr                   thread_;
};

} } // namespace piel::lib
