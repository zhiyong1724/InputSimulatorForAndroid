#ifndef __CONSOLE_H__
#define __CONSOLE_H__
#include <string>
class Console
{
public:
	Console() {}
	virtual ~Console() {}
	virtual void log(const std::string &text) = 0;
};
#endif // !__CONSOLE_H__
