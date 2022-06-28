#include "jhc/config.hpp"

#ifdef JHC_NOT_HEADER_ONLY
#include "../event.hpp"
#endif

#ifdef max
#undef max
#endif

JHC_INLINE jhc::Event::Event(bool isSet) :
    is_set_(isSet) {}

JHC_INLINE void jhc::Event::set() noexcept {
    std::unique_lock<std::mutex> ul(set_mutex_);
    is_set_ = true;
    setted_cond_var_.notify_all();
}

JHC_INLINE void jhc::Event::cancel() noexcept {
    std::unique_lock<std::mutex> ul(set_mutex_);
    is_cancelld_ = true;
    setted_cond_var_.notify_all();
}

JHC_INLINE void jhc::Event::unset() noexcept {
    std::unique_lock<std::mutex> ul(set_mutex_);
    is_set_ = false;
    setted_cond_var_.notify_all();
}

JHC_INLINE bool jhc::Event::isSet() noexcept {
    std::unique_lock<std::mutex> ul(set_mutex_);
    return is_set_;
}

JHC_INLINE bool jhc::Event::isCancelled() noexcept {
    std::unique_lock<std::mutex> ul(set_mutex_);
    return is_cancelld_;
}

JHC_INLINE bool jhc::Event::wait(int64_t millseconds) noexcept {
    std::unique_lock<std::mutex> ul(set_mutex_);
    int64_t m = (millseconds >= 0 ? millseconds : std::chrono::duration_values<int64_t>::max());
    setted_cond_var_.wait_for(ul, std::chrono::milliseconds(m), [this] { return (is_set_ || is_cancelld_); });
    return is_set_;
}