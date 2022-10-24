// COMP710 GP Framework 2022
#ifndef __XINPUTCONTROLLER_H__
#define __XINPUTCONTROLLER_H__

#include "input.h"

#include <vector2.h>
#include <SDL.h>

// Class declaration: 
class XInputController
{
	// Member methods: 
public:
	XInputController();
	~XInputController();
	bool Initialise(int controllerIndex);
	void ProcessInput();
	ButtonState GetButtonState(SDL_GameControllerButton button);
	float GetLeftTrigger() const;
	float GetRightTrigger() const;
	const Vector2& GetLeftStick() const;
	const Vector2& GetRightStick() const;

protected:
private:
	XInputController(const XInputController& xinputcontroller);
	XInputController& operator=(const XInputController& xiputcontroller);
	// Member data: 
public:
protected:
	unsigned char m_previousButtons[SDL_CONTROLLER_BUTTON_MAX];
	unsigned char m_currentButtons[SDL_CONTROLLER_BUTTON_MAX];
	SDL_GameController* m_pController;
	bool m_bConnected;
	Vector2 m_leftStick;
	Vector2 m_rightStick;
	float m_fLeftTrigger;
	float m_fRightTrigger;

private:
};

#endif // __XINPUTCONTROLLER_H__