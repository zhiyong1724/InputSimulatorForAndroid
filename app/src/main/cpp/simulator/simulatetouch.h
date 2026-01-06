#ifndef __SIMULATETOUCH_H__
#define __SIMULATETOUCH_H__
class SimulateTouch
{
public:
    SimulateTouch() {}
    virtual ~SimulateTouch() {}
    virtual void click(int x, int y, int duration) = 0;
    virtual void swipe(int x, int y, int endX, int endY, int duration) = 0;
};
#endif
