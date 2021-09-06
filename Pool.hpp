#ifndef Tools_Thread_Pool_h
#define Tools_Thread_Pool_h

#include <mutex>
#include <thread>
#include <queue>
#include <functional>
#include <condition_variable>

// https://gist.github.com/JoshuaOliphant/6c8ddcd449d4dab211a4
class Pool
{
private:
    using Task_Function_t = std::function<void()>;
    using Mutex_t = std::mutex;
    using Unique_Mutex_Lock_t = std::unique_lock<Mutex_t>;

private:
    std::vector<std::thread> _threads;
    std::queue<Task_Function_t> _task_queue;
    Mutex_t _queue_mutex;
    std::condition_variable _pool_notifier;
    bool _should_stop_processing;
    bool _is_emergency_stop;

public:
    Pool(const std::size_t thread_count)

        : _should_stop_processing(false),
          _is_emergency_stop(false)
    {
        // Sanity
        if (thread_count == 0)
            throw std::runtime_error("ERROR: Thread::Pool() -- must have at least one thread");

        // Init pool
        _threads.reserve(thread_count);

        for (std::size_t i = 0; i < thread_count; ++i)
            _threads.emplace_back([this]()
                                  { Worker(); });
    }

    ~Pool()
    {
    }

    void wait()
    {
        {
            Unique_Mutex_Lock_t queue_lock(_queue_mutex);

            _should_stop_processing = true;
        }

        // Wake up all threads and wait for them to exit
        _pool_notifier.notify_all();

        for (auto &task_thread : _threads)
            task_thread.join();
    }

public:
    template <typename Lambda_t>
    void Add_Task(Lambda_t &&function)
    {
        // Add to task queue
        {
            Unique_Mutex_Lock_t lock(_queue_mutex);

            // Sanity
            if (_should_stop_processing || _is_emergency_stop)
                throw std::runtime_error("ERROR: Thread::Pool::Add_Task() - attempted to add task to stopped pool");

            // Add our task to the queue
            _task_queue.emplace(std::forward<Lambda_t>(function));
        }

        // Notify the pool that there is work to do
        _pool_notifier.notify_one();
    }

    void Emergency_Stop()
    {
        {
            Unique_Mutex_Lock_t queue_lock(_queue_mutex);

            _is_emergency_stop = true;
        }

        _pool_notifier.notify_all();

        for (auto &task_thread : _threads)
            task_thread.join();
    }

public:
    void Worker()
    {
        while (true)
        {
            // Retrieve a task
            Task_Function_t task;
            {
                // Wait on queue or 'stop processing' flags
                Unique_Mutex_Lock_t queue_lock(_queue_mutex);

                _pool_notifier.wait(
                    queue_lock,

                    [this]()
                    { return !_task_queue.empty() || _should_stop_processing || _is_emergency_stop; });

                // Bail when stopped and no more tasks remain,
                // or if an emergency stop has been requested.
                if ((_task_queue.empty() && _should_stop_processing) || _is_emergency_stop)
                    return;

                // Retrieve next task
                task = std::move(_task_queue.front());
                _task_queue.pop();
            }

            // Execute task
            task();
        }
    }
};
#endif