#pragma once

namespace string_helper {

template <typename T>
inline std::string join(const char* prefix, const T& value,
                        const char* suffix) {
  return prefix + std::to_string(value) + suffix;
}

}  // namespace string_helper
