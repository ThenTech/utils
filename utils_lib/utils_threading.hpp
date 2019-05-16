#ifndef UTILS_THREADING_HPP
#define UTILS_THREADING_HPP

#include <mutex>
#include <thread>
#include <future>
#include <condition_variable>

#include <vector>
#include <queue>

#include <functional>
#include <memory>

#include "utils_exceptions.hpp"


#define LOCK_BLOCK(MTX) std::unique_lock<std::mutex> __lock(MTX)

namespace utils::threading {
    /**
     *  \brief  The ThreadPool class
     *
     *          Adapted from https://github.com/progschj/ThreadPool
     *                       https://github.com/jhasse/ThreadPool
     */
    class ThreadPool {
        private:
            // Need to keep track of threads so we can join them
            std::vector<std::thread> workers;

            // The task queue
            std::queue<std::packaged_task<void()>> tasks;

            // Synchronization
            std::mutex queue_mutex;
            std::condition_variable condition;
            bool stop;

        public:
            /**
             *  \brief  Launch \p threads workers that wait for tasks to enqueue.
             *
             *  \param  threads
             *      The amount of worker threads to create.
             */
            inline explicit ThreadPool(size_t threads)
                : stop(false)
            {
                for (size_t i = 0; i < threads; ++i) {
                    this->workers.emplace_back( [this] {
                            while(true) {
                                std::packaged_task<void()> task;

                                {
                                    LOCK_BLOCK(this->queue_mutex);

                                    this->condition.wait(__lock, [this]{
                                        return this->stop || !this->tasks.empty();
                                    });

                                    if (this->stop && this->tasks.empty())
                                        return;

                                    task = std::move(this->tasks.front());
                                    this->tasks.pop();
                                }

                                task();
                            }
                        }
                    );
                }
            }

            inline ~ThreadPool() {
                {
                    LOCK_BLOCK(this->queue_mutex);
                    this->stop = true;
                }

                this->condition.notify_all();

                for (std::thread &worker : this->workers) {
                    worker.join();
                }
            }

            inline size_t size(void) const {
                return this->workers.size();
            }

            inline size_t tasks_in_queue(void) {
                LOCK_BLOCK(this->queue_mutex);
                return this->tasks.size();
            }

            template<
                class F,
                class ...Args,
                class result_type_t = typename std::invoke_result_t<F, Args...>
            >
            std::future<result_type_t> enqueue(F&& f, Args&& ... args) {
                static_assert(std::is_invocable_v<F, Args...>, "ThreadPool::enqueue: Callable function required.");

                std::packaged_task<result_type_t()> task(
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...)
                );

                std::future<result_type_t> res = task->get_future();
                {
                    LOCK_BLOCK(this->queue_mutex);

                    // Don't allow enqueueing after stopping the pool
                    if (this->stop)
                        throw utils::exceptions::Exception("ThreadPool::enqueue",
                                                           "Pool already stopped, cannot enqueue.");

                    this->tasks.emplace(std::move(task));
                }

                this->condition.notify_one();

                return res;
            }
    };
}

#endif // UTILS_THREADING_HPP
