#include "EventLoopThreadPool.h"


EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const std::string& nameArg)
    : baseLoop_(baseLoop),
      name_(nameArg),
      started_(false),
      numThreads_(0),
      next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{

}


void EventLoopThreadPool::start(const ThreadInitCallback& cb)
{
    started_ = true;
    callback_ = cb;

    for (int i = 0; i < numThreads_; ++i)
    {
        char buf[32];
        snprintf(buf, sizeof buf, "%s-%d", name_.c_str(), i);
        EventLoopThread* t = new EventLoopThread(callback_, buf);
        threads_.emplace_back(t);
        loops_.push_back(t->startLoop());
    }

    if (numThreads_ == 0 && callback_)
    {
        callback_(baseLoop_);
    }
}

//如果工作在多线程中，baseLoop默认以轮询的方式分配subloop
EventLoop* EventLoopThreadPool::getNextLoop()
{
    EventLoop* loop = baseLoop_;
    if (!loops_.empty())
    {
        loop = loops_[next_];
        next_ = (next_ + 1) % loops_.size();
    }
    return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{
    if(loops_.empty())
    {
        return std::vector<EventLoop*>(1,baseLoop_);
    }
    else
    {
        return loops_;
    }
}
