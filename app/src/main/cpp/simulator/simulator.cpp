#include "simulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
Simulator::Simulator(SimulateKey* simulateKey, SimulateMouse* simulateMouse, SimulateTouch *simulateTouch, Console* console) : mSimulateKey(simulateKey), mSimulateMouse(simulateMouse), mSimulateTouch(simulateTouch), mConsole(console), mIsReady(false)
{
	mIsRunning.store(false);
	loadCommand();
}

Simulator::~Simulator()
{
	stop();
}

void Simulator::start()
{
    if (!mIsRunning.exchange(true))
    {
        std::lock_guard<std::mutex> autoLock(mLock);
        mSimulatorThread = std::make_shared<std::thread>([&]() {
            std::lock_guard<std::mutex> autoLock(mLock);
            parseScript();
            mSimulatorThread->detach();
            mConsole->log("脚本执行完成。");
            mIsRunning.store(false);
        });
    }
}

void Simulator::stop()
{
    mCondLock.lock();
    mIsReady = true;
    mCond.notify_all();
    mCondLock.unlock();
    std::lock_guard<std::mutex> autoLock(mLock);
    if(mIsRunning.load())
    {
        if (mSimulatorThread->joinable())
        {
            mSimulatorThread->join();
        }
    }
}

bool Simulator::sleep(long ms)
{
    std::unique_lock<std::mutex> autoLock(mCondLock);
    auto end = std::chrono::system_clock::now() + std::chrono::milliseconds(ms);
    while (!mIsReady)
    {
        auto result = mCond.wait_until(autoLock, end);
        if (std::cv_status::timeout == result)
            return true;
    }
    return false;
}

void Simulator::loadCommand()
{
	mCommandMap["delay"] = [&](const cJSON* json) -> bool { return delayCommand(json); };
	mCommandMap["key_down"] = [&](const cJSON* json) -> bool { return keyDownCommand(json); };
	mCommandMap["key_up"] = [&](const cJSON* json) -> bool { return keyUpCommand(json); };
	mCommandMap["key_click"] = [&](const cJSON* json) -> bool { return keyClickCommand(json); };
	mCommandMap["mouse_move"] = [&](const cJSON* json) -> bool { return mouseMoveCommand(json); };
	mCommandMap["mouse_left_click"] = [&](const cJSON* json) -> bool { return mouseLeftClickCommand(json); };
	mCommandMap["mouse_right_click"] = [&](const cJSON* json) -> bool { return mouseRightClickCommand(json); };
	mCommandMap["mouse_left_double_click"] = [&](const cJSON* json) -> bool { return mouseLeftDoubleClickCommand(json); };
	mCommandMap["mouse_right_double_click"] = [&](const cJSON* json) -> bool { return mouseRightDoubleClickCommand(json); };
	mCommandMap["echo"] = [&](const cJSON* json) -> bool { return echoCommand(json); };
	mCommandMap["for"] = [&](const cJSON* json) -> bool { return forCommand(json); };
    mCommandMap["touch"] = [&](const cJSON* json) -> bool { return touchCommand(json); };
    mCommandMap["swipe"] = [&](const cJSON* json) -> bool { return swipeCommand(json); };
}

bool Simulator::loadScript(const std::string& script)
{
	::FILE* file = ::fopen(script.c_str(), "rb");
	if (!file)
	{
		mConsole->log("加载脚本失败, 请检查脚本路径！");
		return false;
	}
	mScript = "";
	char c;
	while (::fread(&c, 1, 1, file) > 0)
	{
		mScript.push_back(c);
	}
	::fclose(file);
	return true;
}

bool Simulator::parseScript()
{
	cJSON* rootJson = cJSON_Parse(mScript.c_str());
	if (!rootJson)
	{
		mConsole->log("语法错误，不是完整的json对象！");
		return false;
	}
	if (!cJSON_IsArray(rootJson))
	{
		mConsole->log("语法错误，根对象应该是个数组！");
		cJSON_free(rootJson);
		return false;
	}
	dispatchCommand(rootJson);
	cJSON_free(rootJson);
	return true;
}

bool Simulator::dispatchCommand(const cJSON* json)
{
	int size = cJSON_GetArraySize(json);
	for (int i = 0; i < size; i++)
	{
		cJSON* itemJson = cJSON_GetArrayItem(json, i);
		if (!cJSON_IsObject(itemJson))
		{
			mConsole->log("语法错误，数组对象应该是个对象！");
			return false;
		}
		cJSON* cmdJson = cJSON_GetObjectItem(itemJson, "cmd");
		if (!cmdJson)
		{
			mConsole->log("语法错误，对象缺少cmd字段！");
			return false;
		}
		if (!cJSON_IsString(cmdJson))
		{
			mConsole->log("语法错误，cmd字段不是字符串！");
			return false;
		}
		auto itr = mCommandMap.find(cJSON_GetStringValue(cmdJson));
		if (itr == mCommandMap.end())
		{
			mConsole->log(std::string("错误的命令：") + cJSON_GetStringValue(cmdJson));
			return false;
		}
		if (!itr->second(itemJson))
			return false;
	}
	return true;
}

bool Simulator::delayCommand(const cJSON* json)
{
	cJSON* durationJson = cJSON_GetObjectItem(json, "duration");
	if (!durationJson || !cJSON_IsNumber(durationJson))
	{
		mConsole->log("命令delay缺少duration字段或者字段类型错误！");
		return false;
	}
	int duration = (int)cJSON_GetNumberValue(durationJson);

    int random = -1;
	cJSON* randomJson = cJSON_GetObjectItem(json, "random");
	if (randomJson && cJSON_IsNumber(randomJson))
	{
        random = (int)cJSON_GetNumberValue(randomJson);
	}
	int ran = random > 0 ? ::rand() % random : 0;
    duration += ran;
    return sleep(duration);
}

bool Simulator::keyDownCommand(const cJSON* json)
{
	cJSON* keyJson = cJSON_GetObjectItem(json, "key");
	if (!keyJson || !cJSON_IsNumber(keyJson))
	{
		mConsole->log("命令key_dowm缺少key字段或者字段类型错误！");
		return false;
	}
	int key = (int)cJSON_GetNumberValue(keyJson);

	int duration = -1;
	cJSON* durationJson = cJSON_GetObjectItem(json, "duration");
	if (durationJson && cJSON_IsNumber(durationJson))
	{
        duration = (int)cJSON_GetNumberValue(durationJson);
	}
	
	int random = -1;
	cJSON* randomJson = cJSON_GetObjectItem(json, "random");
	if (randomJson && cJSON_IsNumber(randomJson))
	{
		random = (int)cJSON_GetNumberValue(randomJson);
	}
	int ran = random > 0 ? ::rand() % random : 0;
    duration += ran;
	mSimulateKey->keyDown(key);
	if (duration >= 0)
	{
        if (!sleep(duration))
            return false;
		mSimulateKey->keyUp(key);
	}
	return true;
}

bool Simulator::keyUpCommand(const cJSON* json)
{
	cJSON* keyJson = cJSON_GetObjectItem(json, "key");
	if (!keyJson || !cJSON_IsNumber(keyJson))
	{
		mConsole->log("命令key_up缺少key字段或者字段类型错误！");
		return false;
	}
	int key = (int)cJSON_GetNumberValue(keyJson);
	mSimulateKey->keyUp(key);
	return true;
}

bool Simulator::keyClickCommand(const cJSON* json)
{
	cJSON* keyJson = cJSON_GetObjectItem(json, "key");
	if (!keyJson || !cJSON_IsNumber(keyJson))
	{
		mConsole->log("命令key_click缺少key字段或者字段类型错误！");
		return false;
	}
	int key = (int)cJSON_GetNumberValue(keyJson);
	mSimulateKey->keyClick(key);
	return true;
}

bool Simulator::mouseMoveCommand(const cJSON* json)
{
	cJSON* xJson = cJSON_GetObjectItem(json, "x");
	if (!xJson || !cJSON_IsNumber(xJson))
	{
		mConsole->log("命令mouse_move缺少x字段或者字段类型错误！");
		return false;
	}
	int x = (int)cJSON_GetNumberValue(xJson);

	cJSON* yJson = cJSON_GetObjectItem(json, "y");
	if (!yJson || !cJSON_IsNumber(yJson))
	{
		mConsole->log("命令mouse_move缺少y字段或者字段类型错误！");
		return false;
	}
	int y = (int)cJSON_GetNumberValue(yJson);

	int offset = -1;
	cJSON* offsetJson = cJSON_GetObjectItem(json, "offset");
	if (offsetJson && cJSON_IsNumber(offsetJson))
	{
		offset = (int)cJSON_GetNumberValue(offsetJson);
	}
	int ran = offset > 0 ? ::rand() % offset : 0;
	x += ran;

	ran = offset > 0 ? ::rand() % offset : 0;
	y += ran;
	mSimulateMouse->MoveTo(x, y);
	return true;
}

bool Simulator::mouseLeftClickCommand(const cJSON* json)
{
	bool ret = true;
	cJSON* xJson = cJSON_GetObjectItem(json, "x");
	if (!xJson || !cJSON_IsNumber(xJson))
	{
		mConsole->log("命令mouse_left_click缺少x字段或者字段类型错误！");
		return false;
	}
	int x = (int)cJSON_GetNumberValue(xJson);

	cJSON* yJson = cJSON_GetObjectItem(json, "y");
	if (!yJson || !cJSON_IsNumber(yJson))
	{
		mConsole->log("命令mouse_left_click缺少y字段或者字段类型错误！");
		return false;
	}
	int y = (int)cJSON_GetNumberValue(yJson);

	int offset = -1;
	cJSON* offsetJson = cJSON_GetObjectItem(json, "offset");
	if (offsetJson && cJSON_IsNumber(offsetJson))
	{
		offset = (int)cJSON_GetNumberValue(offsetJson);
	}
	int ran = offset > 0 ? ::rand() % offset : 0;
	x += ran;

	ran = offset > 0 ? ::rand() % offset : 0;
	y += ran;
	mSimulateMouse->MoveTo(x, y);
	if (!sleep(200))
		return false;
	mSimulateMouse->leftKeyClick();
	return true;
}

bool Simulator::mouseRightClickCommand(const cJSON* json)
{
	cJSON* xJson = cJSON_GetObjectItem(json, "x");
	if (!xJson || !cJSON_IsNumber(xJson))
	{
		mConsole->log("命令mouse_right_click缺少x字段或者字段类型错误！");
		return false;
	}
	int x = (int)cJSON_GetNumberValue(xJson);

	cJSON* yJson = cJSON_GetObjectItem(json, "y");
	if (!yJson || !cJSON_IsNumber(yJson))
	{
		mConsole->log("命令mouse_right_click缺少y字段或者字段类型错误！");
		return false;
	}
	int y = (int)cJSON_GetNumberValue(yJson);

	int offset = -1;
	cJSON* offsetJson = cJSON_GetObjectItem(json, "offset");
	if (offsetJson && cJSON_IsNumber(offsetJson))
	{
		offset = (int)cJSON_GetNumberValue(offsetJson);
	}
	int ran = offset > 0 ? ::rand() % offset : 0;
	x += ran;

	ran = offset > 0 ? ::rand() % offset : 0;
	y += ran;
	mSimulateMouse->MoveTo(x, y);
	if (!sleep(200))
		return false;
	mSimulateMouse->rightKeyClick();
	return true;
}

bool Simulator::mouseLeftDoubleClickCommand(const cJSON* json)
{
	cJSON* xJson = cJSON_GetObjectItem(json, "x");
	if (!xJson || !cJSON_IsNumber(xJson))
	{
		mConsole->log("命令mouse_left_double_click缺少x字段或者字段类型错误！");
		return false;
	}
	int x = (int)cJSON_GetNumberValue(xJson);

	cJSON* yJson = cJSON_GetObjectItem(json, "y");
	if (!yJson || !cJSON_IsNumber(yJson))
	{
		mConsole->log("命令mouse_left_double_click缺少y字段或者字段类型错误！");
		return false;
	}
	int y = (int)cJSON_GetNumberValue(yJson);

	int offset = -1;
	cJSON* offsetJson = cJSON_GetObjectItem(json, "offset");
	if (offsetJson && cJSON_IsNumber(offsetJson))
	{
		offset = (int)cJSON_GetNumberValue(offsetJson);
	}
	int ran = offset > 0 ? ::rand() % offset : 0;
	x += ran;

	ran = offset > 0 ? ::rand() % offset : 0;
	y += ran;
	mSimulateMouse->MoveTo(x, y);
	if (!sleep(200))
		return false;
	mSimulateMouse->leftKeyDoubleClick();
	return true;
}

bool Simulator::mouseRightDoubleClickCommand(const cJSON* json)
{
	cJSON* xJson = cJSON_GetObjectItem(json, "x");
	if (!xJson || !cJSON_IsNumber(xJson))
	{
		mConsole->log("命令mouse_right_double_click缺少x字段或者字段类型错误！");
		return false;
	}
	int x = (int)cJSON_GetNumberValue(xJson);

	cJSON* yJson = cJSON_GetObjectItem(json, "y");
	if (!yJson || !cJSON_IsNumber(yJson))
	{
		mConsole->log("命令mouse_right_double_click缺少y字段或者字段类型错误！");
		return false;
	}
	int y = (int)cJSON_GetNumberValue(yJson);

	int offset = -1;
	cJSON* offsetJson = cJSON_GetObjectItem(json, "offset");
	if (offsetJson && cJSON_IsNumber(offsetJson))
	{
		offset = (int)cJSON_GetNumberValue(offsetJson);
	}
	int ran = offset > 0 ? ::rand() % offset : 0;
	x += ran;

	ran = offset > 0 ? ::rand() % offset : 0;
	y += ran;
	mSimulateMouse->MoveTo(x, y);
	if (!sleep(200))
		return false;
	mSimulateMouse->rightKeyDoubleClick();
	return true;
}

bool Simulator::echoCommand(const cJSON* json)
{
	cJSON* textJson = cJSON_GetObjectItem(json, "text");
	if (!textJson || !cJSON_IsString(textJson))
	{
		mConsole->log("命令echo缺少text字段或者字段类型错误！");
		return false;
	}
	const char *text = cJSON_GetStringValue(textJson);
	mConsole->log(text);
	return true;
}

bool Simulator::forCommand(const cJSON* json)
{
	cJSON* countJson = cJSON_GetObjectItem(json, "count");
	if (!countJson || !cJSON_IsNumber(countJson))
	{
		mConsole->log("命令for缺少count字段或者字段类型错误！");
		return false;
	}
	int count = (int)cJSON_GetNumberValue(countJson);

	cJSON* bodyJson = cJSON_GetObjectItem(json, "body");
	if (!bodyJson || !cJSON_IsArray(bodyJson))
	{
		mConsole->log("命令for缺少body字段或者字段类型错误！");
		return false;
	}
	for (int i = 0; i < count; i++)
	{
		if (!dispatchCommand(bodyJson))
			return false;
	}
	return true;
}

bool Simulator::touchCommand(const cJSON* json)
{
    cJSON* xJson = cJSON_GetObjectItem(json, "x");
    if (!xJson || !cJSON_IsNumber(xJson))
    {
        mConsole->log("命令touch缺少x字段或者字段类型错误！");
        return false;
    }
    int x = (int)cJSON_GetNumberValue(xJson);

    cJSON* yJson = cJSON_GetObjectItem(json, "y");
    if (!yJson || !cJSON_IsNumber(yJson))
    {
        mConsole->log("命令touch缺少y字段或者字段类型错误！");
        return false;
    }
    int y = (int)cJSON_GetNumberValue(yJson);

    int offset = -1;
    cJSON* offsetJson = cJSON_GetObjectItem(json, "offset");
    if (offsetJson && cJSON_IsNumber(offsetJson))
    {
        offset = (int)cJSON_GetNumberValue(offsetJson);
    }
    int ran = offset > 0 ? ::rand() % offset : 0;
    x += ran;

    ran = offset > 0 ? ::rand() % offset : 0;
    y += ran;

    cJSON* durationJson = cJSON_GetObjectItem(json, "duration");
    if (!durationJson || !cJSON_IsNumber(durationJson))
    {
        mConsole->log("命令touch缺少duration字段或者字段类型错误！");
        return false;
    }
    int duration = (int)cJSON_GetNumberValue(durationJson);

	int random = -1;
    cJSON* randomJson = cJSON_GetObjectItem(json, "random");
    if (randomJson && cJSON_IsNumber(randomJson))
    {
        random = (int)cJSON_GetNumberValue(randomJson);
    }
    ran = random > 0 ? ::rand() % random : 0;
    duration += ran;

    mSimulateTouch->click(x, y, duration);
    if (duration >= 0)
    {
        if (!sleep(duration))
            return false;
    }
    return true;
}

bool Simulator::swipeCommand(const cJSON* json)
{
    cJSON* startXJson = cJSON_GetObjectItem(json, "start_x");
    if (!startXJson || !cJSON_IsNumber(startXJson))
    {
        mConsole->log("命令touch_move缺少start_x字段或者字段类型错误！");
        return false;
    }
    int startX = (int)cJSON_GetNumberValue(startXJson);

    cJSON* startYJson = cJSON_GetObjectItem(json, "start_y");
    if (!startYJson || !cJSON_IsNumber(startYJson))
    {
        mConsole->log("命令touch_move缺少start_y字段或者字段类型错误！");
        return false;
    }
    int startY = (int)cJSON_GetNumberValue(startYJson);

    cJSON* durationJson = cJSON_GetObjectItem(json, "duration");
    if (!durationJson || !cJSON_IsNumber(durationJson))
    {
        mConsole->log("命令touch_move缺少duration字段或者字段类型错误！");
        return false;
    }
    int duration = (int)cJSON_GetNumberValue(durationJson);

    cJSON* endXJson = cJSON_GetObjectItem(json, "end_x");
    if (!endXJson || !cJSON_IsNumber(endXJson))
    {
        mConsole->log("命令touch_move缺少end_x字段或者字段类型错误！");
        return false;
    }
    int endX = (int)cJSON_GetNumberValue(endXJson);

    cJSON* endYJson = cJSON_GetObjectItem(json, "end_y");
    if (!endYJson || !cJSON_IsNumber(endYJson))
    {
        mConsole->log("命令touch_move缺少end_y字段或者字段类型错误！");
        return false;
    }
    int endY = (int)cJSON_GetNumberValue(endYJson);

	int startOffset = -1;
    cJSON* startOffsetJson = cJSON_GetObjectItem(json, "start_offset");
    if (startOffsetJson && cJSON_IsNumber(startOffsetJson))
    {
        startOffset = (int)cJSON_GetNumberValue(startOffsetJson);
    }
    int ran = startOffset > 0 ? ::rand() % startOffset : 0;
    startX += ran;

	ran = startOffset > 0 ? ::rand() % startOffset : 0;
    startY += ran;

	int endOffset = -1;
    cJSON* endOffsetJson = cJSON_GetObjectItem(json, "end_offset");
    if (endOffsetJson && cJSON_IsNumber(endOffsetJson))
    {
        endOffset = (int)cJSON_GetNumberValue(endOffsetJson);
    }
    ran = endOffset > 0 ? ::rand() % endOffset : 0;
    endX += ran;

	ran = endOffset > 0 ? ::rand() % endOffset : 0;
    endY += ran;

    int random = -1;
    cJSON* randomJson = cJSON_GetObjectItem(json, "random");
    if (randomJson && cJSON_IsNumber(randomJson))
    {
        random = (int)cJSON_GetNumberValue(randomJson);
    }
    ran = random > 0 ? ::rand() % random : 0;
    duration += ran;
    mSimulateTouch->swipe(startX, startY, endX, endY, duration);
    if (duration >= 0)
    {
        if (!sleep(duration))
            return false;
    }
    return true;
}