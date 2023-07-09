#include <QCoreApplication>
#include <QLocalServer>
#include <QLocalSocket>
#include <QSocketNotifier>
#include <Windows.h>
#include <array>
#include <assert.h>
#include <string_view>

int main(int argc, char **argv) {
  QCoreApplication app{argc, argv};
  QLocalServer local_server;
  local_server.listen("local_socket_socket_notifier");

  QLocalSocket local_socket_client;
  local_socket_client.connectToServer(local_server.serverName());
  local_socket_client.waitForConnected();

  local_server.waitForNewConnection();
  QLocalSocket &local_socket_server = *local_server.nextPendingConnection();

  constexpr std::string_view write_message{"sanitiy check message"};
  auto write_res =
      local_socket_client.write(write_message.data(), write_message.size());
  local_socket_client.flush();
  assert(write_res == write_message.size());

  std::array<char, write_message.size()> read_message_buffer;
  local_socket_server.waitForReadyRead();
  auto read_res = local_socket_server.read(read_message_buffer.data(),
                                           read_message_buffer.size());
  assert(read_res == write_message.size());

  std::string_view read_message{read_message_buffer.data(),
                                read_message_buffer.size()};
  assert(write_message == read_message);

  QSocketNotifier read_notifier{local_socket_server.socketDescriptor(),
                                QSocketNotifier::Type::Read};
  QObject::connect(&read_notifier, &QSocketNotifier::activated,
                   []() { printf("Got message\n"); });
  read_notifier.setEnabled(true);

  local_socket_client.write(write_message.data(), write_message.size());
  local_socket_client.flush();
  printf("Sent message\n");

  return app.exec();
}