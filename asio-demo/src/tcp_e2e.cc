#include <stdio.h>

#include <asio.hpp>
#include <future>

using asio::error_code;
using asio::io_context;
using asio::ip::tcp;

constexpr int kPort = 11111;

static void server_task();
static void client_task();

int main() {
  auto server = std::async(std::launch::async, &server_task);
  auto client = std::async(std::launch::async, &client_task);
  client.get();
  server.get();
  return 0;
}

static void server_task() {
  io_context io;
  auto loop = std::async(std::launch::async, [&io] {
    io_context::work work{io};
    error_code ec;
    io.run(ec);
    printf("event loop exits with %s\n", ec.message().c_str());
  });

  tcp::acceptor acceptor{io, tcp::endpoint{tcp::v4(), kPort}};
  tcp::socket socket{io};
  acceptor.accept(socket);

  error_code ec;
  size_t total_bytes = 0;
  while (true) {
    char buf[1024];
    auto len = socket.read_some(asio::buffer(buf), ec);
    if (ec) {
      printf("read complete: %s\n", ec.message().c_str());
      break;
    }
    buf[len] = '\0';
    total_bytes += len;
  }
  printf("Received %zu bytes\n", total_bytes);

  socket.close(ec);
  acceptor.close(ec);
  io.stop();
  loop.get();
}

static void client_task() {
  io_context io;
  tcp::resolver resolver{io};
  tcp::resolver::query query{"localhost", std::to_string(kPort)};
  tcp::socket socket{io};
  asio::connect(socket, resolver.resolve("localhost", std::to_string(kPort)));
  std::vector<char> buffer(1048576, 'a');
  std::atomic_bool done{false};
  asio::write(socket, asio::const_buffer{buffer.data(), buffer.size()});
}
