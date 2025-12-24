#pragma once
#include "noncopyable.h"
#include "Channel.h"
#include "Socket.h"
#include <functional>
#include "InetAddress.h"
#include "EventLoop.h"



class EventLoop; //前置声明
class InetAddress; //前置声明

class Acceptor : noncopyable
{
public:
    using NewConnectionCallback = std::function<void(int sockfd, const InetAddress& peerAddr)>;

    Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    {
        newConnectionCallback_ = cb;
    }
    bool listenning() const { return listening_; }
    void listen();

private:
    void handleRead();
    EventLoop *loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listening_;
};