#include "pauseui.h"

#include <engine/renderer.h>
#include <engine/sprite.h>
#include <engine/xinputcontroller.h>
#include <fmod.hpp>

#include "scenelevel.h"

const Vector2 screenOrigin = { 0.f, 0.f };

PauseUI::PauseUI()
	: m_fTime(0.f)
	, m_bSelectedExit(false)
{

}

PauseUI::~PauseUI()
{
	delete m_pBackground;
	delete m_pHeader;
	delete m_pSelector;
	delete m_pResumeText;
	delete m_pExitText;

	m_pBackground = nullptr;
	m_pHeader = nullptr;
	m_pSelector = nullptr;
	m_pResumeText = nullptr;
	m_pExitText = nullptr;
}

bool
PauseUI::Initialise(Renderer& renderer, SceneLevel& scene, FMOD::System& fmodSystem)
{
	m_pSceneLevel = &scene;
	m_pFMODsystem = &fmodSystem;

	m_pHeader = renderer.CreateStaticTextSprite("PAUSED", FontSize::LARGE, { 255, 255, 255, 255 }, SpriteOrigin::CENTER);
	m_pSelector = renderer.CreateSprite("assets\\sprites\\ui\\bar.png", SpriteOrigin::CENTER);
	m_pSelector->SetBlueTint(0.6f);
	m_pSelector->SetGreenTint(0.6f);
	m_pSelector->SetRedTint(0.6f);
	m_pSelector->SetScaleX(5.f);
	m_pSelector->SetScaleY(2.f);

	m_pResumeText = renderer.CreateStaticTextSprite("Resume", FontSize::MEDIUM, { 255, 255, 255, 255 }, SpriteOrigin::CENTER);
	m_pExitText = renderer.CreateStaticTextSprite("Quit", FontSize::MEDIUM, { 255, 255, 255, 255 }, SpriteOrigin::CENTER);

	m_pBackground = renderer.CreateSprite("assets\\sprites\\ui\\bar.png", SpriteOrigin::TOP_LEFT);
	m_pBackground->SetScale(static_cast<float>(renderer.GetWidth()) / m_pBackground->GetWidth());
	m_pBackground->SetBlueTint(0.f);
	m_pBackground->SetGreenTint(0.f);
	m_pBackground->SetRedTint(0.f);
	m_pBackground->SetAlpha(0.4f);

	fmodSystem.createSound("assets\\sounds\\select.wav", FMOD_DEFAULT, NULL, &m_pSelectSound);

	return true;
}


void
PauseUI::Process(float deltaTime, Input& input)
{
	static XInputController* controller = input.GetController(0);

	m_fTime += deltaTime;
	m_pSelector->SetAlpha((sinf(m_fTime * static_cast<float>(M_PI))) / 2.f + 0.5f);

	if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_DOWN) == BS_PRESSED ||
		controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_UP) == BS_PRESSED)
	{
		m_pFMODsystem->playSound(m_pSelectSound, NULL, false, &m_pSelectSoundChannel);
		m_bSelectedExit = !m_bSelectedExit;
	}

	if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_A) == BS_PRESSED)
	{
		m_pFMODsystem->playSound(m_pSelectSound, NULL, false, &m_pSelectSoundChannel);
		if (m_bSelectedExit)
		{
			m_pSceneLevel->Exit();
			return;
		}
		else
		{
			m_pSceneLevel->TogglePause();
		}
	}
}

void 
PauseUI::Draw(Renderer& renderer)
{
	m_pBackground->Draw(renderer, screenOrigin, 0.f, DrawSpace::SCREEN_SPACE);

	Vector2 headerPos = { static_cast<float>(renderer.GetWidth()) / 2.f, 300.f };
	m_pHeader->Draw(renderer, headerPos, 0.f, DrawSpace::SCREEN_SPACE);

	Vector2 selectorPos = { renderer.GetWidth() / 2.f, 450.f + (m_bSelectedExit ? 75.f : 0.f) };
	m_pSelector->Draw(renderer, selectorPos, 0.f, DrawSpace::SCREEN_SPACE);

	Vector2 resumeTextPos = { renderer.GetWidth() / 2.f, 450.f };
	m_pResumeText->Draw(renderer, resumeTextPos, 0.f, DrawSpace::SCREEN_SPACE);

	Vector2 exitTextPos = { renderer.GetWidth() / 2.f, 525.f };
	m_pExitText->Draw(renderer, exitTextPos, 0.f, DrawSpace::SCREEN_SPACE);
}