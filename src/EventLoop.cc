#include "EventLoop.h"
#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>
#include "Logger.h"
#include <error.h>
#include "Poller.h"
#include "Channel.h"
#include <memory>

//防止一个线程创建多个EventLoop对象
__thread EventLoop* t_loopInThisThread = nullptr;

//定义默认的Poller IO复用的超时时间
const int kPollTimeMs = 10000;

//创建wakeupfd,用notify通知subReactor有新任务到来
int createEventfd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        LOG_FATAL("Failed in eventfd",errno);
    }
    return evtfd;


}

EventLoop::EventLoop()
    : looping_(false),
      quit_(false),
      callingPendingFunctors_(false),
      threadId_(CurrentThread::tid()),
      poller_(Poller::newDefaultPoller(this)),
      wakeupFd_(createEventfd()),
      wakeupChannel_(new Channel(this, wakeupFd_)),
        currentActiveChannel_(nullptr)

{
    LOG_DEBUG("EventLoop created %p in thread %d", this, threadId_);
    if (t_loopInThisThread)
    {
        LOG_FATAL("Another EventLoop %p exists in this thread %d", t_loopInThisThread, threadId_);
    }
    else
    {
        t_loopInThisThread = this;
    }

    //设置wakeupfd_对应的channel的读事件回调函数
    wakeupChannel_->setReadCallback(
        std::bind(&EventLoop::handleRead, this));
    //监听wakeupChannel_的读事件
    wakeupChannel_->enableReading();

}



EventLoop::~EventLoop()
{
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop()
{
    looping_ = true;
    quit_ = false;

    LOG_INFO("EventLoop %p start looping", this);

    while (!quit_)
    {
        activeChannels_.clear();
        //监听IO事件，返回发生事件的channel列表
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        for (Channel* channel : activeChannels_)
        {
            currentActiveChannel_ = channel;
            //调用channel的事件处理函数
            currentActiveChannel_->handleEvent(pollReturnTime_);
        }
        currentActiveChannel_ = nullptr;
        //执行当前EventLoop需要执行的回调操作
        /*

        */
        doPendingFunctors();
    }

    LOG_INFO("EventLoop %p stop looping", this);
    looping_ = false;

}

void EventLoop::quit(){
    quit_ = true;
    //如果是在其他线程调用的quit，则通过wakeup唤醒loop所在的线程
    if (!isInLoopThread())
    {
        wakeup();
    }
}



//在当前loop中执行cb
void EventLoop::runInLoop(Functor cb)
{
    if(isInLoopThread())
    {
        cb();
    }
    else
    {
        queueInLoop(std::move(cb));
    }
}
//把cb放入队列，唤醒loop所在的线程，执行cb
void EventLoop::queueInLoop(Functor cb)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pendingFunctors_.emplace_back(std::move(cb));
    }

    if(!isInLoopThread() || callingPendingFunctors_)
    {
        wakeup();
    }

}

//唤醒loop所在的线程,向wakeupFd_写一个数据
void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR("EventLoop::wakeup() writes %ld bytes instead of 8", n);
    }

}

//EventLoop的方法，调用Poller的方法
void EventLoop::updateChannel(Channel* channel)
{
    poller_->updateChannel(channel);   
}
void EventLoop::removeChannel(Channel* channel)
{
    poller_->removeChannel(channel);
}
bool EventLoop::hasChannel(Channel* channel)
{
    return poller_->hasChannel(channel);
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = ::read(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR("EventLoop::handleRead() reads %ld bytes instead of 8", n);
    }
}
void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for (const Functor& functor : functors)
    {
        functor();
    }
    callingPendingFunctors_ = false;
}
