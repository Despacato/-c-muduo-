#include "Socket.h"
#include "InetAddress.h"
#include <netinet/in.h>  // for sockaddr_in
#include <netinet/tcp.h> // for TCP_NODELAY
#include <cstring>       // for std::memset
#include <unistd.h>      // for close
#include "Logger.h"

Socket::~Socket()
{
    close(sockfd_);
}

void Socket::bindAddress(const InetAddress& localaddr)
{
    
    if (0 != ::bind(sockfd_, (sockaddr*)localaddr.getSockAddr(), sizeof(sockaddr_in)))
    {
        LOG_FATAL("Socket::bindAddress %d fail\n", sockfd_);
    }
}

void Socket::listen()
{
    if (0 != ::listen(sockfd_, 1024))
    {
        LOG_FATAL("Socket::listen sockfd %d fail\n", sockfd_);
    }
}

int Socket::accept(InetAddress* peeraddr)
{
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof addr); // 替换 bzero 为 std::memset
    socklen_t addrlen = sizeof addr;
    int connfd = ::accept(sockfd_, reinterpret_cast<struct sockaddr*>(&addr), &addrlen);
    if (connfd >= 0)
    {
        peeraddr->setSockAddr(addr);
    }
    return connfd;
}

void Socket::shutdownWrite()
{
    if (::shutdown(sockfd_, SHUT_WR) < 0)
    {
        LOG_ERROR("Socket::shutdownWrite error\n");
    }
}

void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof optval); // 确保包含 <netinet/tcp.h>
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
}

void Socket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof optval);    
}

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof optval);
}