#include "InetAddress.h"
#include <strings.h>

InetAddress::InetAddress(uint16_t port,std::string ip){
    bzero(&addr_,sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
}

InetAddress::InetAddress(const struct sockaddr_in& addr)
    : addr_(addr)
{}

std::string InetAddress::toIp() const{
    char buf[INET_ADDRSTRLEN] = {0};
    const char* result = inet_ntop(AF_INET,&addr_.sin_addr,buf,sizeof buf);
    if(result){
        return std::string(buf);
    }else{
        return "";  
    }
}

uint16_t InetAddress::toPort() const{
    return ntohs(addr_.sin_port);
}

std::string InetAddress::toIpPort() const{
    return toIp() + ":" + std::to_string(toPort());
}



