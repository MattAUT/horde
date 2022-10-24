// COMP710 GP Framework 2022
#ifndef __INPUT_H__
#define __INPUT_H__

class XInputController;

#include <vector2.h>
#include <SDL.h>

enum ButtonState
{
	BS_NEUTRAL,
	BS_PRESSED,
	BS_RELEASED,
	BS_HELD
};

class Input
{
	// Member Methods: 
public:
	Input();
	~Input();
	bool Initialise();
	void ProcessInput();
	// Keyboard:
	ButtonState GetKeyState(SDL_Scancode key);

	// Mouse:
	const Vector2& GetMousePosition() const;
	const Vector2& GetMouseScrollWheel() const;
	ButtonState GetMouseButtonState(int button);
	void ShowMouseCursor(bool show);
	void SetRelativeMode(bool relative);

	// XInput Controllers:
	int GetNumberOfControllersAttached() const;
	XInputController* GetController(int controllerIndex);

protected:
	void ProcessMouseWheel(SDL_Event& event);

private:
	Input(const Input& input);
	Input& operator=(const Input& input) {}

	// Member Data: 
public:
protected:
	unsigned char m_previousKeyboardState[SDL_NUM_SCANCODES];
	const unsigned char* m_pCurrentKeyboardState;
	Vector2 m_mousePosition;
	Vector2 m_mouseWheel;
	unsigned int m_previousMouseButtons;
	unsigned int m_currentMouseButtons;
	bool m_bRelativeMouseMode;
	XInputController* m_pXInputController;
	int m_iNumAttachedControllers;
private:
};
#endif // __INPUT_H__