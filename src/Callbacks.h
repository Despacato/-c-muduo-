#pragma once
#include <memory>
#include <functional>


class Buffer;
class TcpConnection;
class Timestamp;
class EventLoop;  

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;
using HighWaterMarkCallback = std::function<void(const std::shared_ptr<TcpConnection>&, size_t)>;
using MessageCallback = std::function<void(const std::shared_ptr<TcpConnection>&, 
                                           Buffer*, 
                                           Timestamp)>;
using ThreadInitCallback = std::function<void(EventLoop*)>;
