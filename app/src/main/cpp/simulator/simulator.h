#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__
#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include "simulatekey.h"
#include "simulatemouse.h"
#include "simulatetouch.h"
#include "console.h"
#include "cJSON.h"
#include <map>
#include <functional>
class Simulator
{
public:
	Simulator(SimulateKey *simulateKey, SimulateMouse *simulateMouse, SimulateTouch *simulateTouch, Console *console);
	~Simulator();
	void start();
	void stop();
	bool loadScript(const std::string& script);
private:
	void loadCommand();
	bool parseScript();
	bool dispatchCommand(const cJSON *json);
	bool delayCommand(const cJSON* json);
	bool keyDownCommand(const cJSON* json);
	bool keyUpCommand(const cJSON* json);
	bool keyClickCommand(const cJSON* json);
	bool mouseMoveCommand(const cJSON* json);
	bool mouseLeftClickCommand(const cJSON* json);
	bool mouseRightClickCommand(const cJSON* json);
	bool mouseLeftDoubleClickCommand(const cJSON* json);
	bool mouseRightDoubleClickCommand(const cJSON* json);
	bool echoCommand(const cJSON* json);
	bool forCommand(const cJSON* json);
    bool touchCommand(const cJSON* json);
    bool swipeCommand(const cJSON* json);
private:
	SimulateKey *mSimulateKey;
	SimulateMouse *mSimulateMouse;
    SimulateTouch *mSimulateTouch;
	Console *mConsole;
	std::atomic_bool mIsRunning;
	std::shared_ptr<std::thread> mSimulatorThread;
	std::string mScript;
	std::map<std::string, std::function<bool(const cJSON* json)>> mCommandMap;
};
#endif