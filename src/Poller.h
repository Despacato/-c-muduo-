#pragma once
#include "noncopyable.h"
#include "Timestamp.h"
#include <vector>
#include <unordered_map>
class EventLoop; //前置声明
class Channel;

//muduo库中多路事件分发器的核心IO复用模块
class Poller : noncopyable{
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop* loop);
    virtual ~Poller()=default;


    //给所有的IO复用保留统一的接口
    virtual Timestamp poll(int timeoutMs,ChannelList* activeChannels) = 0;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;

    //判断参数channel是否在当前Poller中
    bool hasChannel(Channel* channel) const;
    //EventLoop可以通过该静态方法获取默认的Poller
    static Poller* newDefaultPoller(EventLoop* loop);
protected:
    using ChannelMap = std::unordered_map<int,Channel*>;
    ChannelMap channels_; //保存fd到channel的映射关系

private:
    EventLoop* ownerLoop_; //Poller所属的EventLoop事件循环





};