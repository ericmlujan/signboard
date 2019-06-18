#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>

namespace signboard {
// A queue for items of type T
template <typename T> class MultithreadedQueue {
    MultithreadedQueue<T>() = default;

  // Push a new item on the queue
  void push(T &&item) {
    std::lock_guard<std::mutex> guard(_mutex);
    _queue.push(std::move(item));
    // Let anyone who may be watching the dang thing
    _newWork.notify_one();
  }

  // Remove an item from the queue
  // Blocks if the queue is being accessed by any other thread
  std::optional<T> pop() {
    std::optional<T> result;
    std::unique_lock<std::mutex> lock(_mutex);
    // Man, we ain't found shit!!
    if (!_queue.empty()) {
      result.emplace(std::move(_queue.front()));
      _queue.pop();
    }
    return result;
  }

  // We can't move or copy this class for now
  MultithreadedQueue(MultithreadedQueue &&other) = delete;
  MultithreadedQueue(const MultithreadedQueue &other) = delete;

private:
  std::queue<T> _queue;
  std::mutex _mutex;
  std::condition_variable _newWork;
};
} // namespace signboard
