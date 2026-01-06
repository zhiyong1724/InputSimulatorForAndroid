#ifndef __SIMULATEMOUSE_H__
#define __SIMULATEMOUSE_H__
class SimulateMouse
{
public:
	SimulateMouse() {}
	virtual ~SimulateMouse() {}
	virtual void MoveTo(int x, int y) = 0;
	virtual void leftKeyClick() = 0;
	virtual void rightKeyClick() = 0;
	virtual void leftKeyDoubleClick() = 0;
	virtual void rightKeyDoubleClick() = 0;
};
#endif
