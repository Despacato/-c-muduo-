#include "Thread.h"
#include "CurrentThread.h"
#include <semaphore.h>
#include <unistd.h> 
std::atomic<int> Thread::numCreated_(0);

 Thread::Thread(ThreadFunc func, const std::string& name)
    : started_(false),
      joined_(false),
      thread_(nullptr),
      tid_(0),
      func_(std::move(func)),
      name_(name)
{   
    setDefaultName();
}

Thread::~Thread()
{
    if (thread_ && !joined_)
    {
        thread_->detach();
    }
}

void Thread::start()
{
    started_ = true;
    sem_t sem_;
    sem_init(&sem_, 0, 0);
    thread_ = std::shared_ptr<std::thread>(new std::thread([this, &sem_]()
    {
        tid_ = CurrentThread::tid();
        sem_post(&sem_);
        func_();
    }));
    sem_wait(&sem_);
}

int Thread::join()
{
    if (thread_)
    {
        thread_->join();
        joined_ = true;
        return 0;
    }
    return -1;
}   

void Thread::setDefaultName()
{
    int num = ++numCreated_;
    if (name_.empty())
    {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread%d", num);
        name_ = buf;
    }
}