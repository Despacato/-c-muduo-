#include "Channel.h"
#include "Logger.h"
#include "EventLoop.h"
#include <sys/epoll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;


//EventLoop* loop_ 指向该channel所属的EventLoop
Channel::Channel(EventLoop* loop,int fd)
    : loop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      index_(-1),
      tied_(false)
{
}

Channel::~Channel()
{
}

void Channel::tie(const std::shared_ptr<void>& obj)
{
    tie_ = obj;
    tied_ = true;
}

/*
    当改变channel所表示的fd的events事件后,需要调用update()方法,由Channel所属的EventLoop调用Poller的相应方法,注册fd的events事件
    EventLoop => Poller => Channel

*/

void Channel::update()
{
    //通过channel所属的EventLoop,调用Poller的相应方法,注册fd的events事件
    //loop_->updateChannel(this);
}

//在channel所属的EventLoop中，把当前的channel删除掉
void Channel::remove()
{
    //loop_->removeChannel(this);
}

//fd得到poller通知以后,会调用该方法
void Channel::handleEvent(Timestamp receiveTime)
{
    if(tied_)
    {
        std::shared_ptr<void> guard = tie_.lock();
        if(guard)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
}

//根据fd最终发生的事件revents_，调用相应的回调函数
void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    LOG_INFO("channel handleEvent revents_: %d\n",revents_);
    if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
    {
        if(closeCallback_) closeCallback_();
    }

    if(revents_ & EPOLLERR)
    {
        if(errorCallback_) errorCallback_();
    }

    if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
    {
        if(readCallback_) readCallback_(receiveTime);
    }

    if(revents_ & EPOLLOUT)
    {
        if(writeCallback_) writeCallback_();
    }
}