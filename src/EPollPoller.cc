#include "EPollPoller.h"
#include "Logger.h"
#include <errno.h>
#include <cstring>
#include "Channel.h"
#include <unistd.h>


//channel 未添加到poller中
const int kNew = -1; //channel的index =-1
//channel 已添加到poller中
const int kAdded = 1;
//channel 已从poller中删除
const int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop* loop)
    : Poller(loop)
    , epollfd_(::epoll_create1(EPOLL_CLOEXEC))
    , events_(kInitEventListSize)
{
    if(epollfd_ < 0)
    {
        LOG_FATAL("EPollPoller::EPollPoller epoll_create1 error %d",errno);
    }
}

EPollPoller::~EPollPoller() 
{
    ::close(epollfd_);
}

// 重写基类Poller的抽象方法
Timestamp EPollPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
    LOG_INFO("fd total count %lu", channels_.size());
    int numEvents = ::epoll_wait(epollfd_, &*events_.begin(), events_.size(), timeoutMs);
    int saveErrno = errno;  // 先保存 errno
    Timestamp now(Timestamp::now());
    if (numEvents > 0)
    {
        fillActiveChannels(numEvents, activeChannels);
        if (static_cast<size_t>(numEvents) == events_.size())
        {
            events_.resize(events_.size() * 2);
        }
    }
    else if (numEvents ==0)
    {
        LOG_DEBUG("EPollPoller::poll timeout %d ms", timeoutMs);
    }
    else{
        if(saveErrno != EINTR)
        {
            errno = saveErrno;
            LOG_ERROR("EPollPoller::poll error %d",errno);
        }

    }
    return now;
}

//channel update remove => EventLoop updatechannel removechannel => Poller updatechannel removechannel
void EPollPoller::updateChannel(Channel* channel) 
{
    const int index = channel->index();
    LOG_INFO("fd=%d events=%d index=%d", channel->fd(), channel->events(), index);
    if (index == kNew || index == kDeleted)
    {
        int fd = channel->fd();
        if (index == kNew)
        {
            channels_[fd] = channel;
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        int fd = channel->fd();
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::removeChannel(Channel* channel)
{
    int fd = channel->fd();
    channels_.erase(fd);
    int index = channel->index();
    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}

//填写活跃的连接
void EPollPoller::fillActiveChannels(int numEvents,ChannelList* activeChannels) const
{
    for(int i = 0; i < numEvents; ++i)
    {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);//EventLoop就拿到了它的poller返回的所有发生事件的channel列表
    }
}
//更新channel
void EPollPoller::update(int operation,Channel* channel)
{
    struct epoll_event event;
    memset(&event,0,sizeof event);
    int fd = channel->fd();
    event.events = channel->events();
    event.data.ptr = channel;
    if(::epoll_ctl(epollfd_,operation,fd,&event) < 0)
    {
        if(operation == EPOLL_CTL_DEL)
        {
            LOG_ERROR("epoll_ctl del error: %d\n",errno);
        }
        else
        {
            LOG_FATAL("epoll_ctl add/mod error: %d\n",errno);
        }
    }
}