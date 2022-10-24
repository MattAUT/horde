// COMP710 GP Framework 2022

// This include:
#include "logmanager.h"

// Library includes:
#include <Windows.h>
#include <cassert>
#ifdef _DEBUG
#include <imgui/imgui_impl_sdl.h>
#endif // _DEBUG

// Static Members:
LogManager* LogManager::sm_pInstance = 0;

LogManager& 
LogManager::GetInstance()
{
    if (sm_pInstance == 0)
    {
        sm_pInstance = new LogManager();
    }

    return (*sm_pInstance);
}

void 
LogManager::DestroyInstance()
{
    delete sm_pInstance;
    sm_pInstance = 0;
}

LogManager::LogManager()
{

}

LogManager::~LogManager()
{

}

void 
LogManager::Log(std::string message)
{
    OutputDebugStringA(message.c_str());
    OutputDebugStringA("\n"); 
	m_logHistory.push_back(std::pair<std::string, LogType>(message, LogType::INFO));
}

void
LogManager::Error(std::string message)
{
	OutputDebugStringA(message.c_str());
	OutputDebugStringA("\n");
	m_logHistory.push_back(std::pair<std::string, LogType>(message, LogType::WARNING));
	assert(0);
}

#ifdef _DEBUG
void
LogManager::DebugDraw()
{
	ImGui::Text("Logs");
	ImGui::BeginChild("Log", ImVec2(ImGui::GetWindowContentRegionWidth(), 200), true, ImGuiWindowFlags_HorizontalScrollbar);

	std::reverse(m_logHistory.begin(), m_logHistory.end());

	std::vector<std::pair<std::string, LogType>>::iterator iter = m_logHistory.begin();

	while (iter != m_logHistory.end())
	{
		ImGui::PushStyleColor
		(
			ImGuiCol_Text, iter->second == INFO ? 
			IM_COL32(255, 255, 255, 255) : 
			IM_COL32(255, 0, 0, 255)
		);

		ImGui::Text(iter->first.c_str());
		ImGui::PopStyleColor();
		++iter;
	}

	std::reverse(m_logHistory.begin(), m_logHistory.end());

	ImGui::EndChild();
}
#endif // _DEBUG
