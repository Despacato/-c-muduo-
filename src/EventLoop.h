#pragma once
#include "noncopyable.h"
#include <functional>
#include <atomic>
#include "Timestamp.h"
#include <vector>
#include <memory>
#include <mutex>
#include "CurrentThread.h"

class Channel;
class Poller;

//时间循环类，主要包含了两大模块 Channel Poller(epoll的抽象)
class EventLoop : noncopyable
{
    public:
        using Functor = std::function<void()>;
        EventLoop();
        ~EventLoop();
        void loop();
        void quit();

        Timestamp pollReturnTime() const { return pollReturnTime_; }

        //在当前loop中执行cb
        void runInLoop(Functor cb);
        //把cb放入队列，唤醒loop所在的线程，执行cb
        void queueInLoop(Functor cb);

        //唤醒loop所在的线程
        void wakeup();

        //EventLoop的方法，调用Poller的方法
        void updateChannel(Channel* channel);
        void removeChannel(Channel* channel);
        bool hasChannel(Channel* channel);

        //判断EventLoop是否在自己的线程中
        bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

    private:
        void handleRead(); //wakeupfd_可读时的回调
        void doPendingFunctors(); //执行回调操作

        using ChannelList = std::vector<Channel*>;
        std::atomic<bool> looping_;  
        std::atomic<bool> quit_; //标志退出loop循环
        std::atomic<bool> callingPendingFunctors_; //标志当前是否有需要执行的回调操作
        const pid_t threadId_; //记录当前loop所在线程的id
        Timestamp pollReturnTime_; //poller返回发生事件的channels的时间点
        ChannelList activeChannels_;
        std::unique_ptr<Poller> poller_;

        int wakeupFd_; //主要作用，当mainLoop获取一个新用户的channel,通过轮询算法选择一个subloop,通过wakeupFd_唤醒subLoop处理channel
        std::unique_ptr<Channel> wakeupChannel_;
        Channel* currentActiveChannel_;

        std::vector<Functor> pendingFunctors_; //存储loop需要执行的回调操作
        std::mutex mutex_; //保护pendingFunctors_的线程安全操作
};
