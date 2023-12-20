/**
 * ****************udp.hh****************
 * @brief  封装UDP
 * @author https://github.com/TheOneImmortal
 * @date   2023-10-27
 * ********************************
 */

#ifndef __XN_SU__UDP_HH__
#define __XN_SU__UDP_HH__

/*----------------include----------------*/
#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <psdk_inc/_ip_types.h>
#include <queue>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "../../base-network/include_me.hh"
#include "../../base-type/include_me.hh"
#include "../../id-length-protocol/include_me.hh"
#include "protocol_helper.hh"

/*----------------body----------------*/
namespace xn {

inline u64 threadid_to_number(const std::thread::id &id) {
  return std::hash<std::thread::id>()(id);
}

template <class ReciveHandler, class SendHandler> struct SmartUdp;
template <class... ReciveIs, class... SendIs>
class SmartUdp<idl_protocol::Protocol<ReciveIs...>,
               idl_protocol::Protocol<SendIs...>> {
  using recive_protocol = idl_protocol::Protocol<ReciveIs...>;
  using send_protocol = idl_protocol::Protocol<SendIs...>;

  using recive_handler = idl_protocol::Handler<recive_protocol>;
  using send_handler = idl_protocol::Handler<send_protocol>;

  struct sock_buffer {
    sockaddr_in s;
    BufferOnHeap buffer;
  };

  class SmartUdpQueue {
    std::mutex msg_mtx;
    std::queue<sock_buffer> msg_que;
    std::condition_variable msg_cond;

  public:
    // 消息处理线程，无消息时，进入休眠状态，不占用资源
    u32 size() {
      msg_mtx.lock();
      auto size = msg_que.size();
      msg_mtx.unlock();

      return size;
    }

    // 另一个线程的回调函数，通过这个函数发送消息
    void push(const sockaddr_in &s, BufferOnHeap &&buffer) {
      msg_mtx.lock();
      msg_que.push({s, std::move(buffer)});
      msg_mtx.unlock();

      msg_cond.notify_one();
    }

    sock_buffer pop() {
      msg_mtx.lock();
      auto s_b = std::move(msg_que.front());
      msg_que.pop();
      msg_mtx.unlock();

      return s_b;
    }

    sock_buffer wait_till_pop() {
      std::unique_lock<std::mutex> lk(msg_mtx);
      if (msg_que.empty())
        msg_cond.wait(lk);

      auto s_b = std::move(msg_que.front());
      msg_que.pop();

      return s_b;
    }
  };

public:
  void on(u16 port) { on_helper(htons(port)); }

  void on() {
    sdp_thread = new std::thread(sdp_thread_call, this, 0);

    std::cout << "sdp-thread's id is [{}].\n"_f(
        threadid_to_number(sdp_thread->get_id()));
  }

  void send(const sockaddr_in &s, BufferOnHeap &&buffer) {
    send_queue.push(s, std::move(buffer));
    buffer.data = nullptr;
  }

  void send(const sockaddr_in &s, const auto &...vars) {
    send(s, idl_protocol::serialize<send_protocol>(vars...));
  }

  bool try_recive_one() {
    if (recive_queue.size() <= 0)
      return false;

    auto sock_buffer = recive_queue.pop();
    idl_protocol::parser<recive_handler>(sock_buffer.s, sock_buffer.buffer);
    return true;
  }

  ~SmartUdp() { delete sdp_thread; }

private:
  void on_helper(u16 port_n) {
    sdp_thread = new std::thread(sdp_thread_call, this, port_n);

    std::cout << "sdp-thread's id is [{}].\n"_f(
        threadid_to_number(sdp_thread->get_id()));
  }

  void sdp_thread_call(const u16 port_n) {
    { // 网络启动
      u16 version_requested = u8_u8_to_u16(2, 2);
      WSADATA wsaData;

      i32 err = WSAStartup(version_requested, &wsaData);
      if (err) {
        std::cout << "WSAStartup failed with error: {}\n"_f(err);
        exit(1);
      }
      std::cout << "WSAStartup!\n";
    }

    // SOCKET 申请
    SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == INVALID_SOCKET) {
      std::cout << "socket failed with error: {}"_f(WSAGetLastError());
      exit(1);
    }
    std::cout << "[{}] my thread is [{}]\n"_f(
        s, threadid_to_number(std::this_thread::get_id()));

    // 绑定端口
    if (port_n) {
      sockaddr_in sin{
          .sin_family = AF_INET, // TODO: IPV6使用AF_INET6
          .sin_port = port_n,
          .sin_addr = {.S_un = {.S_addr = static_cast<u_long>(ip_any_n)}},
      };
      i32 err = bind(s, (sockaddr *)&sin, sizeof(sockaddr));
      if (err)
        std::cout << "[{}] **bind faild** error-code: {}\n"_f(s, err);
      else
        std::cout << "[{}] bind port {} success!\n"_f(s, ntohs(port_n));
    }

    // 取消阻塞
    ioctlsocket(s, FIONBIO, as_pointer<u_long>((u_long)1));

    std::cout << "[{}] sdp send-recive thread start.\n"_f(s);
    // 无尽的收发循环
    while (1) {
      // 接收信息
      while (1) {
        Buffer<1024> recv_buffer;
        sockaddr_in other;

        i32 recv_len = recvfrom(s, as_pointer<char>(recv_buffer.data),
                                recv_buffer.size, 0, (sockaddr *)&other,
                                as_pointer<i32>((i32)sizeof(sockaddr)));

        // 返回值小于零，接收出现错误
        // 目前就当收完了，忽略其它错误
        if (recv_len < 0) {
          // std::cout << "[{}] recvfrom failed with error: {}\n"_f(s,
          // recv_len); std::cout << "[{}] more info: {}\n"_f(s,
          // WSAGetLastError());
          break;
        }

        // std::cout << "[{}] recive msg from {}:{}\n"_f(
        //     s, inet_ntoa(other.sin_addr), ntohs(other.sin_port));

        BufferOnHeap actual_buffer(recv_len);
        memcpy(as_pointer<char>(actual_buffer.data),
               as_pointer<char>(recv_buffer.data), recv_len);

        // std::cout << "[{}] recive msg: {}\n"_f(s,
        // BufferOnHad(actual_buffer));

        recive_queue.push(other, std::move(actual_buffer));
      }

      // 发送信息
      while (1) {
        if (send_queue.size() == 0)
          break;

        auto send_msg = send_queue.pop();
        // std::cout << "[{}] sending msg...\n"_f(s);
        i32 err = sendto(s, (char *)send_msg.buffer.data, send_msg.buffer.size,
                         0, (sockaddr *)&send_msg.s, sizeof(sockaddr));
        if (err < 0) {
          std::cout << "[{}] sendto failed with err {}!\n"_f(s, err);
          std::cout << "[{}] more info: {}!\n"_f(s, WSAGetLastError());
        }
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    closesocket(s);
    WSACleanup();
  }

private:
  SmartUdpQueue send_queue;
  SmartUdpQueue recive_queue;

  std::thread *sdp_thread;
};

} // namespace xn

#endif