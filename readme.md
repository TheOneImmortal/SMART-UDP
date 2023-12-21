# 更“聪明”的UDP

## 编译

请使用CMAKE进行编译，
本地适配的话，请参照已有的CMakeLists.txt文件进行自行配置。

推荐使用VSCode，因为我用的VSCode。


## 样例代码
```
using namespace xn;

// 类型声明，交由协议使用
enum class Answer : i32;
enum class UserID : i32;
enum class Password : i32;
struct UserState;
struct ErrorMsg;

// 协议声明，
// 无法直接使用，必须交由处理器使用，使用方法参考下文
using ctos_ptc = idl_protocol::Protocol<idl_protocol::Item<ErrorMsg>,
                                        idl_protocol::Item<char, i32, i32>,
                                        idl_protocol::Item<UserID, Password>>;
using stoc_ptc = idl_protocol::Protocol<idl_protocol::Item<ErrorMsg>,
                                        idl_protocol::Item<Answer>,
                                        idl_protocol::Item<UserID, UserState>>;

// 具体类型定义，交由处理器使用
struct UserState {
  v2r32 pos;
};
struct ErrorMsg {
  u32 err_code;
};

sockaddr_in server_addr{
    .sin_family = AF_INET,
    .sin_port = htons(17771),
    .sin_addr = {.S_un = {.S_addr =
                              [] {
                                in_addr in;
                                inet_pton(AF_INET, "127.0.0.1", &in);
                                return in.S_un.S_addr;
                              }()}},
};

SmartUdp<ctos_ptc, stoc_ptc> server;
SmartUdp<stoc_ptc, ctos_ptc> client;

// 定义处理器，定义时必须知晓所有需要处理的类型
// 具体定义方式参考如下
template <> class idl_protocol::Handler<ctos_ptc> {
public:
  static void recived(sockaddr_in addr, ErrorMsg err) {
    std::cout << "server-recived errorcode:{} from {}:{}\n"_f(
        err.err_code, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
  }

  static void recived(sockaddr_in addr, char operate, i32 a, i32 b) {
    std::cout << "server-recived operate:{} a:{} b:{}\n"_f(operate, a, b);

    switch (operate) {
    case '+': {
      server.send(addr, Answer{a + b});
    } break;
    case '-': {
      server.send(addr, Answer{a - b});
    } break;
    }
  }

  static void recived(sockaddr_in addr, UserID user, Password pass) {
    std::cout << "server-recived login, [id]{} [password]{}\n"_f((i32)user,
                                                                 (i32)pass);
    srand(time(nullptr));
    server.send(addr, user,
                UserState{.pos = {.x = 10000000 * r32(rand()) / r32(INT_MAX),
                                  .y = 10000000 * r32(rand()) / r32(INT_MAX)}});
  }
};
template <> class idl_protocol::Handler<stoc_ptc> {
public:
  static void recived(sockaddr_in addr, ErrorMsg err) {
    std::cout << "client-recived errorcode:{} from {}:{}\n"_f(
        err.err_code, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
  }

  static void recived(sockaddr_in addr, Answer ans) {
    std::cout << "client-recived ans:{}\n"_f(i32(ans));
  }

  static void recived(sockaddr_in addr, UserID user, UserState state) {
    std::cout << "client-recived state update, [id]{} [position]{}\n"_f(
        (i32)user, state.pos);
  }
};

void server_call() {
  std::cout << "server starting...\n";
  server.on(17771);
  std::cout << "server started\n";

  while (1) {
    bool is_recived = server.try_recive_one();

    if (!is_recived)
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}

void client_call() {
  std::cout << "client starting...\n";
  client.on();
  std::cout << "client started\n";

  client.send(server_addr, UserID{12345}, Password{54321});

  client.send(server_addr, '+', 12, 21);
  client.send(server_addr, '-', 12, 21);

  while (1) {
    bool is_recived = client.try_recive_one();

    if (!is_recived)
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
}

int main() {
  std::cout << "----------main----------\n";

  std::thread server_thread(server_call);
  std::thread client_thread(client_call);

  server_thread.join();
  client_thread.join();

  std::cout << "----------main over----------\n";
}
```
