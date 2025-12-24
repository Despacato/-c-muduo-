#pragma once
#include "noncopyable.h"
#include <functional>
#include "Timestamp.h"
#include <memory>

class EventLoop; //前置声明
/*
    Channel 封装了sockfd和其它感兴趣的event,如EPOLLIN,EPOLLOUT等
    Channel 还负责调用相应的回调函数,如readCallback_, writeCallback
    还绑定了poller返回的具体事件
*/

class Channel : noncopyable
{
    public:
        using EventCallback = std::function<void()>;
        using ReEventCallback = std::function<void(Timestamp)>;
        Channel(EventLoop* loop,int fd);
        ~Channel();

        void handleEvent(Timestamp receiveTime);

        void setReadCallback(const ReEventCallback& cb){ readCallback_ = std::move(cb); }

        void setWriteCallback(const EventCallback& cb){ writeCallback_ = std::move(cb); }

        void setCloseCallback(const EventCallback& cb){ closeCallback_ = std::move(cb); }   
        void setErrorCallback(const EventCallback& cb){ errorCallback_ = std::move(cb); }

        //防止当channel被手动remove掉之后,它还在执行回调操作
        void tie(const std::shared_ptr<void>&);
        int fd() const { return fd_; }
        int events() const { return events_; }

        void set_revents(int revt) { revents_ = revt; } //used by Poller

        

        //设置fd当前的事件状态
        void enableReading() { events_ |= kReadEvent; update(); }
        void disableReading() { events_ &= ~kReadEvent; update(); }
        void enableWriting() { events_ |= kWriteEvent; update(); }
        void disableWriting() { events_ &= ~kWriteEvent; update(); }
        void disableAll() { events_ = kNoneEvent; update(); }

        //返回fd当前的事件状态
        bool isWriting() const { return events_ & kWriteEvent; }
        bool isReading() const { return events_ & kReadEvent; }
        bool isNoneEvent() const { return events_ == kNoneEvent; }

        int index() { return index_; }
        void set_index(int idx) { index_ = idx; }

        //one loop per thread
        EventLoop* ownerLoop() { return loop_; }
        void remove();

    private:

        void update();
        void handleEventWithGuard(Timestamp receiveTime);
        
        static const int kNoneEvent;
        static const int kReadEvent;
        static const int kWriteEvent;
        EventLoop* loop_; //Channel属于哪个EventLoop
        const int fd_; //Channel负责的fd
        int events_; //注册感兴趣的事件
        int revents_; //poller返回的具体事件
        int index_; //在poller中的状态，未添加、已添加、已删除

        std::weak_ptr<void> tie_;
        bool tied_;


        // 因为channel通道里面能够获知fd最终发生的事件revents_，所以它负责调用具体的回调操作
        ReEventCallback readCallback_;
        EventCallback writeCallback_;
        EventCallback closeCallback_;
        EventCallback errorCallback_;
        
};
