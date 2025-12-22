#pragma once

class EventLoop
{
    public:
        EventLoop();
        void loop();
    private:
        bool looping_;
};
