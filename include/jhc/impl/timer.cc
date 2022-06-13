#include "jhc/config.hpp"

#ifdef JHC_NOT_HEADER_ONLY
#include "../timer.hpp"
#endif

#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <vector>
#include <stack>
#include <set>

namespace jhc {
namespace timer_detail {
// The event structure that holds the information about a timer.
struct Event {
    std::size_t id;
    std::chrono::time_point<std::chrono::steady_clock> start;
    std::chrono::microseconds period;
    Timer::handler_t handler;
    bool valid;
    Event() :
        id(0),
        start(std::chrono::microseconds::zero()),
        period(std::chrono::microseconds::zero()),
        handler(nullptr),
        valid(false) {
    }

    template <typename Func>
    Event(std::size_t id, std::chrono::time_point<std::chrono::steady_clock> start, std::chrono::microseconds period, Func&& handler) :
        id(id), start(start), period(period), handler(std::forward<Func>(handler)), valid(true) {
    }

    Event(Event&& r) = default;
    Event& operator=(Event&& ev) = default;
    Event(const Event& r) = delete;
    Event& operator=(const Event& r) = delete;
};

// A time event structure that holds the next timeout and a reference to its Event struct.
struct Time_event {
    std::chrono::time_point<std::chrono::steady_clock> next;
    std::size_t ref;
};

inline bool operator<(const Time_event& l, const Time_event& r) {
    return l.next < r.next;
}
}  // namespace timer_detail

class Timer::Private {
   public:
    // Thread and locking variables.
    std::mutex m_;
    std::condition_variable cond_;
    std::thread worker_;

    // Use to terminate the timer thread.
    bool done_ = false;

    // The vector that holds all active events.
    std::vector<timer_detail::Event> events_;
    // Sorted queue that has the next timeout at its top.
    std::multiset<timer_detail::Time_event> time_events_;

    // A list of ids to be re-used. If possible, ids are used from this pool.
    std::stack<std::size_t> free_ids_;
};

Timer::Timer() :
    p_(new Private()) {
    std::unique_lock<std::mutex> lock(p_->m_);
    p_->worker_ = std::thread([this] { run(); });
}

Timer::~Timer() {
    std::unique_lock<std::mutex> lock(p_->m_);
    p_->done_ = true;
    lock.unlock();
    p_->cond_.notify_all();
    p_->worker_.join();
    p_->events_.clear();
    p_->time_events_.clear();
    while (!p_->free_ids_.empty()) {
        p_->free_ids_.pop();
    }

    delete p_;
    p_ = nullptr;
}

std::size_t Timer::add(
    const std::chrono::time_point<std::chrono::steady_clock>& when,
    handler_t&& handler,
    const std::chrono::microseconds& period) {
    std::unique_lock<std::mutex> lock(p_->m_);
    std::size_t id = 0;
    // Add a new event.
    // Prefer an existing and free id. If none is available, add a new one.
    if (p_->free_ids_.empty()) {
        id = p_->events_.size();
        timer_detail::Event e(id, when, period, std::move(handler));
        p_->events_.push_back(std::move(e));
    }
    else {
        id = p_->free_ids_.top();
        p_->free_ids_.pop();
        timer_detail::Event e(id, when, period, std::move(handler));
        p_->events_[id] = std::move(e);
    }
    p_->time_events_.insert(timer_detail::Time_event{when, id});
    lock.unlock();
    p_->cond_.notify_all();
    return id;
}

std::size_t Timer::add(const uint64_t afterMicroseconds, handler_t&& handler, const uint64_t periodMicroseconds) {
    return add(std::chrono::microseconds(afterMicroseconds), std::move(handler), std::chrono::microseconds(periodMicroseconds));
}

bool Timer::remove(std::size_t id) {
    std::unique_lock<std::mutex> lock(p_->m_);
    if (p_->events_.size() == 0 || p_->events_.size() <= id) {
        return false;
    }
    p_->events_[id].valid = false;
    auto it = std::find_if(p_->time_events_.begin(), p_->time_events_.end(),
                           [&](const timer_detail::Time_event& te) { return te.ref == id; });
    if (it != p_->time_events_.end()) {
        p_->free_ids_.push(it->ref);
        p_->time_events_.erase(it);
    }
    lock.unlock();
    p_->cond_.notify_all();
    return true;
}

void Timer::run() {
    std::unique_lock<std::mutex> lock(p_->m_);

    while (!p_->done_) {
        if (p_->time_events_.empty()) {
            // Wait for work
            p_->cond_.wait(lock);
        }
        else {
            timer_detail::Time_event te = *(p_->time_events_.begin());
            if (std::chrono::steady_clock::now() >= te.next) {
                // Remove time event
                p_->time_events_.erase(p_->time_events_.begin());

                // Invoke the handler
                lock.unlock();
                p_->events_[te.ref].handler(te.ref);
                lock.lock();

                if (p_->events_[te.ref].valid && p_->events_[te.ref].period.count() > 0) {
                    // The event is valid and a periodic timer.
                    te.next += p_->events_[te.ref].period;
                    p_->time_events_.insert(te);
                }
                else {
                    // The event is either no longer valid because it was removed in the
                    // callback, or it is a one-shot timer.
                    p_->events_[te.ref].valid = false;
                    p_->free_ids_.push(te.ref);
                }
            }
            else {
                p_->cond_.wait_until(lock, te.next);
            }
        }
    }
}
}  // namespace jhc