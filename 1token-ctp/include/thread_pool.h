#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

namespace onetoken {

using Task = std::function<void()>;

class ThreadPool {
 public:
  ThreadPool(uint32_t thread_num = 4) {
    stopped_ = false;
    thread_num_ = thread_num;
  }

  ~ThreadPool() {
    stopped_.store(true);
    waiting_cv_.notify_all();
    for (auto& thread : pool_) {
      if (thread->joinable()) {
        thread->join();
      }
    }
  }

  void Start();
  void Work();
  void Join();

  template <class F, class... Args>
  bool AddTask(F&& f, Args&&... args) {
    if (stopped_.load()) {
      return false;
    }
    using RetType = decltype(f(args...));
    auto task = std::make_shared<std::packaged_task<RetType()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    task_futures_.emplace_back(std::move(task->get_future()));
    {
      std::lock_guard<std::mutex> lock{mutex_};
      waiting_tasks_.emplace([task]() { (*task)(); });
    }
    waiting_cv_.notify_one();
    return true;
  }

  int idle_thread_num() { return idle_thread_num_; }
  void set_thread_num(uint32_t thread_num) { thread_num_ = thread_num; }

 private:
  std::vector<std::shared_ptr<std::thread>> pool_;
  std::queue<Task> waiting_tasks_;
  std::vector<std::future<void>> task_futures_;
  std::mutex mutex_;
  std::condition_variable waiting_cv_;
  std::atomic<bool> stopped_;
  uint32_t thread_num_;
  std::atomic<uint32_t> idle_thread_num_;
};

}  // namespace onetoken
