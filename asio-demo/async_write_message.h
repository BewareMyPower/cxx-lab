#pragma once

#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::tcp;

// The return type of the initiating function is deduced from the combination of
// CompletionToken type and the completion handler's signature. When the
// completion token is a simple callback, the return type is void. However, when
// the completion token is boost::asio::yield_context (used for stackful
// coroutines) the return type would be std::size_t, and when the completion
// token is boost::asio::use_future it would be std::future<std::size_t>.
//
// In C++14 we can omit the return type as it is automatically deduced from the
// return type of our underlying asynchronous operation
template <typename CompletionToken>
inline auto asyncWrite(tcp::socket& socket, const std::string& message,
                       CompletionToken&& token) {
  // When delegating to the underlying operation we must take care to perfectly
  // forward the completion token. This ensures that our operation works
  // correctly with move-only function objects as callbacks, as well as other
  // completion token types.
  return boost::asio::async_write(
      socket, boost::asio::buffer(message.c_str(), message.size()),
      std::forward<CompletionToken>(token));
}
