#pragma once
#include "noncopyable.h"
#include "EventLoopThread.h"
#include <vector>
#include <string>
#include <memory>
#include <functional>


class EventLoop;

class EventLoopThreadPool : noncopyable
{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    EventLoopThreadPool(EventLoop* baseLoop, const std::string& nameArg);
    ~EventLoopThreadPool();

    void setThreadNum(int numThreads) { numThreads_ = numThreads; }

    void start(const ThreadInitCallback& cb = ThreadInitCallback());

    //如果工作在多线程中，baseLoop默认以轮询的方式分配subloop
    EventLoop* getNextLoop();

    std::vector<EventLoop*> getAllLoops();

    bool started() const { return started_; }
private:

    EventLoop* baseLoop_; //用户传入的baseloop，mainLoop
    std::string name_; //线程池名字
    bool started_; //线程池是否启动
    int numThreads_; //线程池中的线程数量
    int next_; //下一个被分配的subloop索引
    std::vector<std::unique_ptr<EventLoopThread>> threads_; //存放EventLoopThread的智能指针
    std::vector<EventLoop*> loops_; //存放subloop的EventLoop指针
    ThreadInitCallback callback_; //loop初始化的回调函数

};

