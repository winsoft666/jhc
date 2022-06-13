#include "jhc/config.hpp"

#ifdef JHC_NOT_HEADER_ONLY
#include "../event.hpp"
#endif

JHC_INLINE jhc::Event::Event(bool setted) :
    setted_(setted) {}

JHC_INLINE void jhc::Event::set() noexcept {
    std::unique_lock<std::mutex> ul(setted_mutex_);
    setted_ = true;
    setted_cond_var_.notify_all();
}

JHC_INLINE void jhc::Event::unset() noexcept {
    std::unique_lock<std::mutex> ul(setted_mutex_);
    setted_ = false;
    setted_cond_var_.notify_all();
}

JHC_INLINE bool jhc::Event::isSetted() noexcept {
    std::unique_lock<std::mutex> ul(setted_mutex_);
    return setted_;
}

JHC_INLINE bool jhc::Event::wait(int32_t millseconds) noexcept {
    std::unique_lock<std::mutex> ul(setted_mutex_);
    setted_cond_var_.wait_for(ul, std::chrono::milliseconds(millseconds),
                              [this] { return setted_; });
    return setted_;
}