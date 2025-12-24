#pragma once
#include "Poller.h"
#include <vector>
#include <sys/epoll.h>
#include "Timestamp.h"

class Channel;
class EventLoop;

/**
    epoll的使用
    epoll_create1
    epoll_ctl update remove (add/mod/del)
    epoll_wait poll

*/

class EPollPoller : public Poller
{
public:
    EPollPoller(EventLoop* loop);
    ~EPollPoller() override;

    // 重写基类Poller的抽象方法
    Timestamp poll(int timeoutMs,ChannelList* activeChannels) override;
    void updateChannel(Channel* channel) override;
    void removeChannel(Channel* channel) override;

private:

    static const int kInitEventListSize = 16;
    using EventList = std::vector<struct epoll_event>;

    //填写活跃的连接
    void fillActiveChannels(int numEvents,ChannelList* activeChannels) const;
    //更新channel
    void update(int operation,Channel* channel);

    int epollfd_;
    EventList events_;

};