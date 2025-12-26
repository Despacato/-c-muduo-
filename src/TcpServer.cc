#include "TcpServer.h"
#include "Logger.h"
#include "EventLoopThreadPool.h"
#include "TcpConnection.h"  // 添加这行，包含完整定义
#include <functional>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cassert>


EventLoop* CheckLoopNotNull(EventLoop* loop){
    if(loop == nullptr){
        LOG_FATAL("%s:%s:%d mainLoop is null \n",__FILE__,__FUNCTION__,__LINE__);
    }
    return loop;
}
TcpServer::TcpServer(EventLoop* loop, const InetAddress &listenAddr,
                     const std::string& nameArg, Option option)
    : loop_(CheckLoopNotNull(loop)),
      ipPort_(listenAddr.toIpPort()),
      name_(nameArg),
      acceptor_(new Acceptor(loop, listenAddr, option == kReusePort)),
      // 添加这一行：初始化 threadPool_
      threadPool_(new EventLoopThreadPool(loop, nameArg + "ThreadPool")),
      connectionCallback_(),
      messageCallback_(),
      nextConnId_(1)
{
    acceptor_->setNewConnectionCallback(
        std::bind(&TcpServer::newConnection, this,
                  std::placeholders::_1, std::placeholders::_2));
}


void TcpServer::setThreadNum(int numThreads){
    threadPool_->setThreadNum(numThreads);
}

void TcpServer::start()
{
    int expected = 0;
    // 使用原子操作确保只执行一次
    if (started_.compare_exchange_strong(expected, 1)) {
        threadPool_->start(threadInitCallback_);
        loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
        std::cout << "TcpServer " << name_ << " started on " << ipPort_ << std::endl;
    } else {
        std::cout << "TcpServer " << name_ << " already started" << std::endl;
    }
}
void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr)
{
    EventLoop *ioLoop = threadPool_->getNextLoop();
    char buf[64] = {0};
    snprintf(buf, sizeof buf, "-%s#%d" , ipPort_.c_str(), nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;

    LOG_INFO("TcpServer::newConnection [%s] - new connection [%s] from %s \n",
        name_.c_str(), connName.c_str(), peerAddr.toIpPort().c_str());

    // 通过sockfd获取其绑定的本机的ip地址和端口信息
    sockaddr_in local;
    ::bzero(&local, sizeof local);
    socklen_t addrlen = sizeof local;
    if(::getsockname(sockfd, (sockaddr*)&local, &addrlen) < 0)
    {
        LOG_ERROR("sockets::getLocalAdddr");
    }
    InetAddress localAddr(local);

    // 根据链接成功的sockfd, 创建TcpConnection链接对象
    TcpConnectionPtr conn(new TcpConnection(
                        ioLoop,
                        connName,
                        sockfd,
                        localAddr,
                        peerAddr));
    connections_[connName] = conn;
    // 下面的回调都是用户设置给TcpServer的 =》TcpConnection=>>Channel =》Poller
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);

    // 设置如何关闭链接的回调
    conn->setCloseCallback(
        std::bind(&TcpServer::removeConnection, this, std::placeholders::_1)
    );

    // 直接调用connectEstablished
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}
TcpServer::~TcpServer()
{
    for (auto &item : connections_)
    {
        // 这个局部的shared_ptr 智能指针对象，出右括号后可以自动释放new 出来的TcpConnection资源
        TcpConnectionPtr conn(item.second);
        item.second.reset();

        // 销毁链接
        conn->getLoop()->runInLoop(
            std::bind(&TcpConnection::connectDestroyed, conn)
        );
    }
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn)
{
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}
void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn)
{
    LOG_INFO("TcpServer::removeConnectionInLoop [%s] - connection %s \n",
        name_.c_str(), conn->name().c_str());
    
    connections_.erase(conn->name());
    EventLoop *ioLoop = conn->getLoop();
    ioLoop->queueInLoop(
        std::bind(&TcpConnection::connectDestroyed, conn)
    );
}