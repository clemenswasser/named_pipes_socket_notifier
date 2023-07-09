#include <QCoreApplication>
#include <QSocketNotifier>
#include <Windows.h>
#include <array>
#include <assert.h>
#include <string_view>

int main(int argc, char **argv) {
  constexpr std::wstring_view pipe_name{
      L"\\\\.\\pipe\\named_pipes_socket_notifier"};
  auto named_pipe_server =
      CreateNamedPipeW(pipe_name.data(), PIPE_ACCESS_DUPLEX,
                       PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT |
                           PIPE_REJECT_REMOTE_CLIENTS,
                       PIPE_UNLIMITED_INSTANCES, 8 * 1024, 8 * 1024,
                       NMPWAIT_USE_DEFAULT_WAIT, nullptr);
  assert(named_pipe_server != INVALID_HANDLE_VALUE);

  auto named_pipe_client =
      CreateFileW(pipe_name.data(), GENERIC_READ | GENERIC_WRITE,
                  FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
                  FILE_ATTRIBUTE_NORMAL, nullptr);
  assert(named_pipe_client != INVALID_HANDLE_VALUE);

  constexpr std::string_view write_message{"sanitiy check message"};
  auto write_res = WriteFile(named_pipe_server, write_message.data(),
                             write_message.size(), nullptr, nullptr);
  assert(write_res != 0);

  std::array<char, write_message.size()> read_message_buffer;
  auto read_res = ReadFile(named_pipe_client, read_message_buffer.data(),
                           read_message_buffer.size(), nullptr, nullptr);
  assert(read_res != 0);

  std::string_view read_message{read_message_buffer.data(),
                                read_message_buffer.size()};
  assert(write_message == read_message);

  QCoreApplication app{argc, argv};

  QSocketNotifier read_notifier{(qintptr)named_pipe_client,
                           QSocketNotifier::Type::Read};
  QObject::connect(&read_notifier, &QSocketNotifier::activated,
                   []() { printf("Got message\n"); });
  read_notifier.setEnabled(true);

  write_res = WriteFile(named_pipe_server, write_message.data(),
                        write_message.size(), nullptr, nullptr);
  assert(write_res != 0);
  printf("Sent message\n");

  return app.exec();
}