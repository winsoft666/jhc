#ifdef JHC_NOT_HEADER_ONLY
#include "../event.hpp"
#endif

jhc::Event::Event(bool setted) :
    setted_(setted) {}

void jhc::Event::set() noexcept {
    std::unique_lock<std::mutex> ul(setted_mutex_);
    setted_ = true;
    setted_cond_var_.notify_all();
}

void jhc::Event::unset() noexcept {
    std::unique_lock<std::mutex> ul(setted_mutex_);
    setted_ = false;
    setted_cond_var_.notify_all();
}

bool jhc::Event::isSetted() noexcept {
    std::unique_lock<std::mutex> ul(setted_mutex_);
    return setted_;
}

bool jhc::Event::wait(int32_t millseconds) noexcept {
    std::unique_lock<std::mutex> ul(setted_mutex_);
    setted_cond_var_.wait_for(ul, std::chrono::milliseconds(millseconds),
                              [this] { return setted_; });
    return setted_;
}