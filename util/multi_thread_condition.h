#ifndef GENART_UTIL_MULTI_THREAD_CONDITION_H
#define GENART_UTIL_MULTI_THREAD_CONDITION_H

#include <condition_variable>
#include <mutex>

namespace util {

class MultiThreadCondition {
 public:
  MultiThreadCondition() : done_(false) {}

  // Wait for any thread to trigger the condition.
  void Wait() {
    std::unique_lock<std::mutex> l(m_);
    while (!done_) {
      cv_.wait(l);
    }
  }

  // Notify the waiting thread that we're finished.
  void Notify() {
    std::unique_lock<std::mutex> l(m_);
    done_ = true;
    cv_.notify_one();
  }
  
 private:
  std::mutex m_;
  std::condition_variable cv_;
  bool done_;
};


}  // namespace util

#endif
