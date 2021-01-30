/**
 * 线程池（C++简单实现）
 * @file: ThreadPool.h
 * @author: rancheng <rc4work@163.com>
 * @date: 2021-01-30
 */
#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <utility>
#include <thread>
#include <condition_variable>
#include <functional>
#include "Guards.h"

namespace dev {

class ThreadPool {
public:
    using SP = std::shared_ptr<ThreadPool>;

    // 创建线程池
    explicit ThreadPool(unsigned threadNum) {
        // 创建线程
        for (unsigned i = 0; i < threadNum; ++i) {
            m_pool.emplace_back([this] {
                while (true) {
                    // 取出任务出来执行
                    std::function<void()> task;
                    {
                        UniqueLock lock(m_mutex);
                        m_cv.wait(lock, [this] { return !m_runFlag || !m_taskQueue.empty(); });
                        if (!m_runFlag) {
                            // 停止运行了
                            return;
                        }
                        task = std::move(m_taskQueue.front());
                        m_taskQueue.pop();
                    }
                    task();
                }
            });
        }
    }

    // 析构函数
    ~ThreadPool() {
        {
            Guard  guard(m_mutex);
            m_runFlag = false;
        }
        m_cv.notify_all();
        for (auto& t : m_pool) {
            t.join();
        }
    }

    // 添加任务
    template <class F>
    void enqueue(F&& f) {
        {
            Guard  guard(m_mutex);
            m_taskQueue.push(std::forward<F>(f));
        }
        m_cv.notify_one();
    }

private:
    // 线程池
    std::vector<std::thread> m_pool;

    // 同步变量
    Mutex m_mutex;
    std::condition_variable m_cv;

    // 任务队列
    std::queue<std::function<void()>> m_taskQueue;

    // 运行标志
    bool m_runFlag = true;
};

}   // namespace dev