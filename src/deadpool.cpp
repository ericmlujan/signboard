#include "deadpool.h"

namespace signboard {
Deadpool::Deadpool(size_t numJobs) : _shutdown(false) {
  for (size_t i = 0; i < numJobs; ++i) {
    _workers.emplace_back(DeadpoolThread(*this));
  }
}

Deadpool::~Deadpool() {
  // Tell all threads that they should shut down
  _shutdown = true;
  _newJobs.notify_all();

  // Join to the main thread
  for (auto &worker : _workers) {
    worker.join();
  }
}

void DeadpoolThread::operator()() {
  std::function<void()> job;
  for (;;) {
    {
      std::unique_lock<std::mutex> guard(_pool._mutex);
      while (!_pool._shutdown && _pool._jobs.empty()) {
        _pool._newJobs.wait(guard);
      }

      if (_pool._shutdown) {
        return;
      }

      job = std::move(_pool._jobs.front());
      _pool._jobs.pop_front();
    }
    job();
  }
}

} // namespace signboard
