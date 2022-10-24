// COMP710 GP Framework 2022

#include "scenemenu.h"

#include <engine/renderer.h>
#include <engine/sprite.h>
#include <engine/scenemanager.h>
#include <engine/xinputcontroller.h>

#include <cassert>
#include <utils.h>
#include <cmath>
#include <fmod.hpp>
#ifdef _DEBUG
#include <imgui/imgui_impl_sdl.h>
#endif // _DEBUG

#include "smokeemitter.h"

SceneMenu::SceneMenu()
	: m_fTime(0.f)
	, m_state(MENU::MAIN_MENU)
	, m_bSelectedExit(false)
{
#ifdef _DEBUG
	dm_name = "Menu Scene";
#endif // _DEBUG
}

SceneMenu::~SceneMenu()
{
	if (m_bIsLoaded)
	{
		Deinitialise();
	}
}

bool SceneMenu::Initialise(Renderer& renderer, Camera& camera, SceneManager& sceneManager, FMOD::System& fmodSystem)
{
	m_pHeader = renderer.CreateStaticTextSprite("HORDE", FontSize::LARGE, { 255, 255, 255, 255 }, SpriteOrigin::CENTER);
	m_pSelector = renderer.CreateSprite("assets\\sprites\\ui\\bar.png", SpriteOrigin::CENTER);
	m_pSelector->SetBlueTint(0.6f);
	m_pSelector->SetGreenTint(0.6f);
	m_pSelector->SetRedTint(0.6f);

	m_pPlayText = renderer.CreateStaticTextSprite("PLAY", FontSize::MEDIUM, { 255, 255, 255, 255 }, SpriteOrigin::CENTER);
	m_pExitText = renderer.CreateStaticTextSprite("QUIT", FontSize::MEDIUM, { 255, 255, 255, 255 }, SpriteOrigin::CENTER);

	m_pLVL1Thumb = renderer.CreateSprite("assets\\sprites\\ui\\lvl1thumb.png", SpriteOrigin::CENTER);
	m_pLVL1Thumb->SetScale(0.4f);
	
	m_pLVL2Thumb = renderer.CreateSprite("assets\\sprites\\ui\\lvl2thumb.png", SpriteOrigin::CENTER);
	m_pLVL2Thumb->SetScale(0.4f);

	fmodSystem.createSound("assets\\sounds\\select.wav", FMOD_DEFAULT, NULL, &m_pSelectSound);

	m_fTime = 0.f;
	m_state = MENU::MAIN_MENU;
	m_bSelectedExit = false;

	renderer.SetClearColour(10, 14, 16);

	return Scene::Initialise(renderer, camera, sceneManager, fmodSystem);
}

void
SceneMenu::Deinitialise()
{
	delete m_pHeader;
	delete m_pSelector;
	delete m_pPlayText;
	delete m_pExitText;
	delete m_pLVL1Thumb;
	delete m_pLVL2Thumb;

	m_pHeader = nullptr;
	m_pSelector = nullptr;
	m_pPlayText = nullptr;
	m_pExitText = nullptr;
	m_pLVL1Thumb = nullptr;
	m_pLVL2Thumb = nullptr;

	Scene::Deinitialise();
}

void SceneMenu::Process(float deltaTime, Input& input)
{
	m_fTime += deltaTime;
	m_pSelector->SetAlpha((sinf(m_fTime * static_cast<float>(M_PI))) / 2.f + 0.5f);

	static XInputController* controller = input.GetController(0);
	if (input.GetNumberOfControllersAttached() > 0)
	{
		if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_B) == BS_PRESSED &&
			m_state == MENU::LEVEL_SELECT)
		{
			m_state = MENU::MAIN_MENU;
			m_bSelectedExit = false;
		}

		if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_DOWN) == BS_PRESSED ||
			controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_UP) == BS_PRESSED ||
			controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_LEFT) == BS_PRESSED ||
			controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == BS_PRESSED)
		{
			m_bSelectedExit = !m_bSelectedExit;
			m_pFMODsystem->playSound(m_pSelectSound, NULL, false, &m_pSelectSoundChannel);
		}

		if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_A) == BS_PRESSED)
		{
			m_pFMODsystem->playSound(m_pSelectSound, NULL, false, &m_pSelectSoundChannel);
			 
			if (m_state == MENU::MAIN_MENU)
			{
				if (m_bSelectedExit)
				{
					m_pSceneManager->Exit();
					return;
				}
				else
				{
					m_state = MENU::LEVEL_SELECT;
				}
			}
			else if (m_state == MENU::LEVEL_SELECT)
			{
				if (m_bSelectedExit)
				{
					m_pSceneManager->LoadLevel(2);
					return;
				}
				else
				{
					m_pSceneManager->LoadLevel(1);
					return;
				}
			}
		}
	}

	if (m_state == MENU::MAIN_MENU)
	{
		m_pSelector->SetScaleX(5.f);
		m_pSelector->SetScaleY(2.f);
	}
	else if (m_state == MENU::LEVEL_SELECT)
	{
		m_pSelector->SetScaleX(7.f);
		m_pSelector->SetScaleY(7.f);
	}
}

void SceneMenu::Draw(Renderer& renderer)
{
	Vector2 selectorPos = m_state == MENU::MAIN_MENU ?
		Vector2(renderer.GetWidth() / 2.f, static_cast<float>(renderer.GetHeight() / 2) + (m_bSelectedExit ? 50.f : -50.f)) :
		Vector2(renderer.GetWidth() / 2.f + (m_bSelectedExit ? 150.f : -150.f), renderer.GetHeight() / 2.f);

	m_pSelector->Draw(renderer, selectorPos, 0.f, DrawSpace::SCREEN_SPACE);

	if (m_state == MENU::MAIN_MENU)
	{
		Vector2 headerPos = { renderer.GetWidth() / 2.f, 150.f };
		m_pHeader->Draw(renderer, headerPos, 0.f, DrawSpace::SCREEN_SPACE);

		Vector2 playTextPos = { renderer.GetWidth() / 2.f, static_cast<float>(renderer.GetHeight() / 2) - 50.f};
		m_pPlayText->Draw(renderer, playTextPos, 0.f, DrawSpace::SCREEN_SPACE);

		Vector2 exitTextPos = { renderer.GetWidth() / 2.f, static_cast<float>(renderer.GetHeight() / 2) + 50.f };
		m_pExitText->Draw(renderer, exitTextPos, 0.f, DrawSpace::SCREEN_SPACE);
	}
	else if (m_state == MENU::LEVEL_SELECT)
	{
		Vector2 lvl1ThumbPos = { static_cast<float>(renderer.GetWidth() / 2.f) - 150.f, renderer.GetHeight() / 2.f };
		m_pLVL1Thumb->Draw(renderer, lvl1ThumbPos, 0.f, DrawSpace::SCREEN_SPACE);

		Vector2 lvl2ThumbPos = { static_cast<float>(renderer.GetWidth() / 2.f) + 150.f, renderer.GetHeight() / 2.f };
		m_pLVL2Thumb->Draw(renderer, lvl2ThumbPos, 0.f, DrawSpace::SCREEN_SPACE);
	}
}

void
SceneMenu::Reset()
{

}

#ifdef _DEBUG
void SceneMenu::DebugDraw(Renderer& renderer)
{

}
#endif // _DEBUG
