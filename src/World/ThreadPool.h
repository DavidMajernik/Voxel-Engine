#pragma once

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <future>
using namespace std;

class ThreadPool {

public:

    ThreadPool(int num_threads);

    ThreadPool();

    ~ThreadPool();

    // Template enqueue for tasks that return a value
    template<class F>
    auto enqueue(F&& f) -> std::future<typename std::result_of<F()>::type> {
        using return_type = typename std::result_of<F()>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(std::forward<F>(f));
        std::future<return_type> res = task->get_future();

        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            tasks_.emplace([task]() { (*task)(); });
        }
        cv_.notify_one();
        return res;
    }

private:
    // Vector to store worker threads
    vector<thread> threads_;

    // Queue of tasks
    queue<function<void()> > tasks_;

    // Mutex to synchronize access to shared data
    mutex queue_mutex_;

    // Condition variable to signal changes in the state of
    // the tasks queue
    condition_variable cv_;

    // Flag to indicate whether the thread pool should stop
    // or not
    bool stop_ = false;

};