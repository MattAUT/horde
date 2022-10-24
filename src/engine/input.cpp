// COMP710 GP Framework 2022

#include "input.h"

#include "renderer.h"
#include "logmanager.h"
#include "scene.h"
#include "xinputcontroller.h"

#include <game/game.h>
#include <cassert>
#include <SDL.h>
#include <cstdio>
#ifdef _DEBUG
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#endif // _DEBUG

Input::Input()
	: m_pCurrentKeyboardState(0)
	, m_previousMouseButtons(0)
	, m_currentMouseButtons(0)
	, m_pXInputController(0)
	, m_iNumAttachedControllers(0)
	, m_bRelativeMouseMode(false)
{

}

Input::~Input()
{
	delete[] m_pXInputController;
	m_pXInputController = 0;
}

bool
Input::Initialise()
{
	for (int k = 0; k < SDL_NUM_SCANCODES; ++k)
	{
		m_previousKeyboardState[k] = 0;
	}
	m_pCurrentKeyboardState = SDL_GetKeyboardState(0);
	unsigned int alreadyInit = SDL_WasInit(SDL_INIT_GAMECONTROLLER);
	if (alreadyInit == 0)
	{
		int result = SDL_Init(SDL_INIT_GAMECONTROLLER);
		LogManager::GetInstance().Log("Input System Initalised");
	}

	m_iNumAttachedControllers = SDL_NumJoysticks();

	m_pXInputController = new XInputController[m_iNumAttachedControllers];
	for (int k = 0; k < m_iNumAttachedControllers; ++k)
	{
		if (SDL_IsGameController(k))
		{
			m_pXInputController[k].Initialise(k);
		}
	}
	return true;
}

void
Input::ProcessInput()
{
	
	// Store the old state...
	for (int k = 0; k < SDL_NUM_SCANCODES; ++k)
	{
		m_previousKeyboardState[k] = m_pCurrentKeyboardState[k];
	}

	m_previousMouseButtons = m_currentMouseButtons;
	// Update the mouse state...
	m_mouseWheel.Set(0.0f, 0.0f);
	int mouseX = 0;
	int mouseY = 0;
	if (m_bRelativeMouseMode)
	{
		m_currentMouseButtons = SDL_GetRelativeMouseState(&mouseX, &mouseY);
	}
	else
	{
		m_currentMouseButtons = SDL_GetMouseState(&mouseX, &mouseY);
	}

	m_mousePosition.Set(static_cast<float>(mouseX), static_cast<float>(mouseY));
	for (int k = 0; k < m_iNumAttachedControllers; ++k)
	{
		m_pXInputController[k].ProcessInput();
	}

	SDL_Event event;
	while (SDL_PollEvent(&event) != 0)
	{
#ifdef _DEBUG
		ImGuiIO& io = ImGui::GetIO();
		// Tell ImGui to hide the mouse cursor:
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		if (io.WantCaptureMouse || io.WantCaptureKeyboard)
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			continue;
		}
#endif // _DEBUG

		ProcessMouseWheel(event);
		if (event.type == SDL_QUIT)
		{
			Game::GetInstance().Quit();
		}
	}
}

ButtonState
Input::GetKeyState(SDL_Scancode key)
{
	if (m_previousKeyboardState[key] == 0)
	{
		if (m_pCurrentKeyboardState[key] == 0)
		{
			return BS_NEUTRAL;
		}
		else
		{
			return BS_PRESSED;
		}
	}
	else
	{
		if (m_pCurrentKeyboardState[key] == 0)
		{
			return BS_RELEASED;
		}
		else
		{
			return BS_HELD;
		}
	}
}

void
Input::ShowMouseCursor(bool show)
{
	if (show == true)
	{
		SDL_ShowCursor(1);
	}
	else
	{
		SDL_ShowCursor(0);
	}
}

const Vector2&
Input::GetMousePosition() const
{
	return m_mousePosition;
}

const Vector2&
Input::GetMouseScrollWheel() const
{
	return m_mouseWheel;
}

ButtonState
Input::GetMouseButtonState(int button)
{
	if ((m_previousMouseButtons & SDL_BUTTON(button)) == 0)
	{
		if ((m_currentMouseButtons & SDL_BUTTON(button)) == 0)
		{
			return BS_NEUTRAL;
		}
		else
		{
			return BS_PRESSED;
		}
	}
	else
	{
		if ((m_currentMouseButtons & SDL_BUTTON(button)) == 0)
		{
			return BS_RELEASED;
		}
		else
		{
			return BS_HELD;
		}
	}
}

void
Input::ProcessMouseWheel(SDL_Event& event)
{
	if (event.type == SDL_MOUSEWHEEL)
	{
		m_mouseWheel.x = static_cast<float>(event.wheel.x);
		m_mouseWheel.y = static_cast<float>(event.wheel.y);
	}
}

void
Input::SetRelativeMode(bool relative)
{
	if (relative)
	{
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
	else
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
	m_bRelativeMouseMode = relative;
}

int
Input::GetNumberOfControllersAttached() const
{
	return m_iNumAttachedControllers;
}

XInputController*
Input::GetController(int controllerIndex)
{
	return &m_pXInputController[controllerIndex];
}