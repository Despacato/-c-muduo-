#pragma once
#include "noncopyable.h"

#include "EventLoop.h"
#include "Acceptor.h"
#include "InetAddress.h"
#include <functional>
#include "EventLoopThreadPool.h"
#include <memory>
#include "Callbacks.h"
#include <string>




class TcpServer : noncopyable
{
public:
    using NewConnectionCallback = std::function<void(int sockfd, const InetAddress&)>;
    
    enum Option
    {
        kNoReusePort,
        kReusePort,
    };
    TcpServer(EventLoop* loop,
              const InetAddress& listenAddr,
              const std::string& nameArg,
              Option option = kNoReusePort);
    ~TcpServer();
    void setThreadInitcallback(const ThreadInitCallback &cb) {threadInitCallback_ = cb;}
    void setConnectionCallback(const ConnectionCallback &cb) {connectionCallback_ = cb;}
    void setMessageCallback(const MessageCallback &cb){ messageCallback_ = cb;}
    void setWriteCallback(const WriteCompleteCallback &cb) {writeCompleteCallback_ = cb;}
    void setThreadNum(int numThreads;)

    void start();
private:
    void newConnection(int sockfd,const InetAddress &peerAddr);
    void removeConnection(const TcpConnectionPtr &conn);
    void removeConnectionInLoop(const TcpConnectionPtr &conn);

    using ConnectionMap = std::unordered_map<std::string,TcpConnectionPtr>;
    EventLoop* loop_;
    const std::string inPort_;
    const std::string name_;
    std::unique_ptr<Acceptor> acceptor_; //运行在mainLoop，负责
    std::shared_ptr<EventLoopThreadPool> threadPool_; //subloop线程池

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    ThreadInitCallback threadInitCallback_;

    std::atomic<int> started_;
    int nextConnId_;

    ConnectionMap connections_;
};