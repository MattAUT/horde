// COMP710 GP Framework 2022
#ifndef __LOGMANAGER_H__
#define __LOGMANAGER_H__

#include <vector>
#include <map>

enum LogType
{
	INFO,
	WARNING
};

class LogManager
{
    // Member methods:
public:
    static LogManager& GetInstance();
    static void DestroyInstance();

    void Log(std::string message);
    void Error(std::string message);
#ifdef _DEBUG
	void DebugDraw();
#endif // _DEBUG
protected:

private:
    LogManager();
    ~LogManager();
    LogManager(const LogManager& logManager);
    LogManager& operator=(const LogManager& logManager);

    // Member data:
public:

protected:
    static LogManager* sm_pInstance;
	std::vector<std::pair<std::string, LogType>> m_logHistory;
	bool m_bShowLogMessages;

private:

};

#endif //__LOGMANAGER_H__
