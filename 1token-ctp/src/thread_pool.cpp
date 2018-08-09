#include "thread_pool.h"

namespace onetoken {

void ThreadPool::Start() {
  for (uint32_t i = 0; i < thread_num_; ++i) {
    pool_.emplace_back(
        std::make_shared<std::thread>(std::bind(&ThreadPool::Work, this)));
  }
}

void ThreadPool::Work() {
  while (!this->stopped_) {
    std::function<void()> task;
    {
      std::unique_lock<std::mutex> lock{this->mutex_};
      this->waiting_cv_.wait(lock, [this] {
        return this->stopped_.load() || !this->waiting_tasks_.empty();
      });
      if (this->stopped_ && this->waiting_tasks_.empty()) {
        return;
      }
      task = std::move(this->waiting_tasks_.front());
      this->waiting_tasks_.pop();
    }
    idle_thread_num_--;
    task();
    idle_thread_num_++;
  }
}

void ThreadPool::Join() {
  for (auto& future : task_futures_) {
    future.get();
  }
  task_futures_.clear();
}

}  // namespace onetoken