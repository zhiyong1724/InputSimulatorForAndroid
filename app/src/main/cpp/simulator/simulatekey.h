#ifndef _SIMULATEKEY_H_
#define _SIMULATEKEY_H_
class SimulateKey
{
public:
	SimulateKey() {}
	virtual ~SimulateKey() {}
	virtual void keyDown(int code) = 0;
	virtual void keyUp(int code) = 0;
	virtual void keyClick(int code) = 0;
};
#endif
