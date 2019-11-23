#pragma once

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace signboard {
// DEADPOOL: the sassy, heroic thread pool
class Deadpool {
  friend class DeadpoolThread;

public:
  Deadpool(size_t numJobs);
  ~Deadpool();

  template <typename F, typename... Args> inline void push(F &&f, Args &&... args) {
    {
      std::lock_guard<std::mutex> guard(_mutex);
      _jobs.emplace_back(std::bind(f, args...));
    }
    _newJobs.notify_one();
  }

private:
  bool _shutdown;
  std::mutex _mutex;
  std::condition_variable _newJobs;
  std::deque<std::function<void()>> _jobs;
  std::vector<std::thread> _workers;
};

class DeadpoolThread {
public:
  DeadpoolThread(Deadpool &pool) : _pool(pool) {}
  void operator()();

private:
  Deadpool &_pool;
};

} // namespace signboard
