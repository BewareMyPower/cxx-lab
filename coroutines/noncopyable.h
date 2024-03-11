#pragma once

class noncopyable {
 public:
  noncopyable() = default;
  noncopyable(const noncopyable&) = delete;
  noncopyable& operator=(const noncopyable&) = delete;
  noncopyable(noncopyable&&) noexcept = default;
  noncopyable& operator=(noncopyable&&) noexcept = default;
};
