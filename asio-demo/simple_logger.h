#pragma once

#include <iostream>
#include <thread>

namespace impl {

template <typename T>
inline void log(const char* prefix, T&& arg) {
  std::cout << prefix << std::forward<T>(arg) << std::endl;
}

template <typename T, typename... Args>
inline void log(const char* prefix, T&& first, Args&&... rest) {
  std::cout << prefix << first;
  log(" ", std::forward<Args>(rest)...);
}

inline auto threadId() { return std::this_thread::get_id(); }

}  // namespace impl

template <typename... Args>
inline void log(Args&&... args) {
  impl::log("", std::forward<Args>(args)...);
}

// NOTE: no log level control yet
#define LOG_DEBUG(...) log("[DEBUG]", impl::threadId(), __VA_ARGS__)
#define LOG_INFO(...) log("[INFO]", impl::threadId(), __VA_ARGS__)
#define LOG_WARN(...) log("[WARN]", impl::threadId(), __VA_ARGS__)
#define LOG_ERROR(...) log("[ERROR]", impl::threadId(), __VA_ARGS__)
