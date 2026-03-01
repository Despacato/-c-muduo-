# muduo_test

基于 C++11 移植的 muduo 网络库，完全移除对 Boost 的依赖，使用现代 C++ 标准库重新实现。

## 主要特性

- **零 Boost 依赖**：全部使用 C++11 标准库替代 Boost 组件
- **高性能事件循环**：基于 Reactor 模式，使用 Linux epoll 实现 I/O 多路复用
- **多线程 TCP 服务器**：每线程一个事件循环（One Loop Per Thread），支持多线程并发
- **现代 C++ 语法**：使用智能指针、lambda、`std::function` 等现代特性
- **简洁的 API**：保持 muduo 原有的简洁易用设计风格

## 架构概览

本项目采用 **Reactor 模式**，核心设计原则为"每线程一个事件循环"。

```
TcpServer
├── Acceptor              （主循环：监听并接受新连接）
├── EventLoopThreadPool
│   ├── EventLoopThread → EventLoop   （工作线程 0）
│   ├── EventLoopThread → EventLoop   （工作线程 1）
│   └── ...
└── TcpConnection 映射表
```

新连接通过轮询（Round-Robin）方式分发到各工作线程的事件循环中处理。

### 核心组件

| 组件 | 职责 |
|------|------|
| `EventLoop` | 事件循环驱动核心，管理 Channel 注册与回调执行，提供线程安全的 `runInLoop` / `queueInLoop` 接口 |
| `EPollPoller` | 封装 Linux `epoll`，每次循环返回活跃的 Channel 列表 |
| `Channel` | 封装文件描述符及其关注的事件与回调（读/写/关闭/错误），不持有 fd 所有权 |
| `TcpServer` | 用户侧服务器接口，管理 Acceptor、线程池及所有 TcpConnection |
| `TcpConnection` | 代表一条已建立的 TCP 连接，管理读写缓冲区并触发用户回调 |
| `Acceptor` | 持有监听套接字，接受新连接后通知 TcpServer |
| `Buffer` | 非阻塞 I/O 缓冲区，使用 `readv` + 栈缓冲实现高效读取 |
| `Logger` | 单例日志，支持 INFO / ERROR / FATAL / DEBUG 四个级别 |

## 移除的 Boost 组件及替代方案

| Boost 组件 | C++11 替代方案 | 说明 |
|-----------|--------------|------|
| `boost::function` | `std::function` | 函数对象包装器 |
| `boost::bind` | `std::bind` / lambda | 参数绑定 |
| `boost::shared_ptr` | `std::shared_ptr` | 共享智能指针 |
| `boost::weak_ptr` | `std::weak_ptr` | 弱引用指针 |
| `boost::scoped_ptr` | `std::unique_ptr` | 独占智能指针 |
| `boost::noncopyable` | 删除拷贝构造/赋值运算符 | 不可拷贝基类 |
| `boost::ptr_vector` | `std::vector<std::unique_ptr>` | 指针容器 |

## 环境要求

- **编译器**：g++ 4.8+ 或 clang++ 3.3+（需支持 C++11）
- **操作系统**：Linux（推荐 Ubuntu 16.04+）
- **构建工具**：CMake 3.10+

## 构建项目

```bash
mkdir build && cd build
cmake ..
make
```

构建产物：

- `bin/test` — 测试可执行文件
- `lib/libmuduo_test.so` — 共享库

## 运行测试

```bash
./bin/test
```

## 致谢

- 感谢陈硕（[@chenshuo](https://github.com/chenshuo)）创作的优秀 muduo 网络库
- 参考了 asio、libevent 等优秀网络库的设计思想
