#include "gameendui.h"

#include <engine/renderer.h>
#include <engine/spritesheetanimation.h>

const float fadeInOutTime = 1.25f;

const Vector2 screenOrigin = { 0.f, 0.f };


GameEndUI::GameEndUI()
	: m_bIsLoaded(false)
	, m_bDrawText(false)
{

}

GameEndUI::~GameEndUI()
{
	delete m_pBackground;
	delete m_pForeground;
	delete m_pGameOverText;
	delete m_pActionText;

	m_pBackground = nullptr;
	m_pForeground = nullptr;
	m_pGameOverText = nullptr;
	m_pActionText = nullptr;
}

bool
GameEndUI::Initialise(Renderer& renderer)
{
	m_pGameOverText = renderer.CreateStaticTextSprite("GAME OVER!", FontSize::LARGE, { 255, 255, 255, 255 }, SpriteOrigin::CENTER);
	m_pActionText = renderer.CreateStaticTextSprite("Press START to return to the main menu", FontSize::MEDIUM, { 255, 255, 255, 255 }, SpriteOrigin::CENTER);

	m_pBackground = renderer.CreateSprite("assets\\sprites\\ui\\bar.png", SpriteOrigin::TOP_LEFT);
	m_pBackground->SetScale(static_cast<float>(renderer.GetWidth()) / m_pBackground->GetWidth());
	m_pBackground->SetBlueTint(0.f);
	m_pBackground->SetGreenTint(0.f);
	m_pBackground->SetRedTint(0.3f);
	m_pBackground->SetAlpha(0.f);

	m_pForeground = renderer.CreateSprite("assets\\sprites\\ui\\bar.png", SpriteOrigin::TOP_LEFT);
	m_pForeground->SetScale(static_cast<float>(renderer.GetWidth()) / m_pForeground->GetWidth());
	m_pForeground->SetBlueTint(0.f);
	m_pForeground->SetGreenTint(0.f);
	m_pForeground->SetRedTint(0.3f);
	m_pForeground->SetAlpha(0.f);
	return true;
}

bool
GameEndUI::Setup(Statistics stats)
{
	sprintf_s(m_sRoundText, "You Survived %d Round%c", stats.round, stats.round != 1 ? 's' : '\0');
	sprintf_s(m_sTimeText, "Time Survived: %.0f:%02d", stats.playTime / 60.f, static_cast<int>(stats.playTime) % 60);
	sprintf_s(m_sKillsText, "Kills: %d", stats.killCount);

	m_bIsLoaded = true;
	return m_bIsLoaded;
}

void
GameEndUI::Process(float deltaTime)
{
	float backgroundAlpha = m_pBackground->GetAlpha();
	float foregroundAlpha = m_pForeground->GetAlpha();

	if (backgroundAlpha < 1.f)
	{
		float alpha = backgroundAlpha + (deltaTime / fadeInOutTime );
		m_pBackground->SetAlpha(alpha);
		m_pForeground->SetAlpha(alpha);
	}
	else if (foregroundAlpha > 0.f)
	{
		m_pForeground->SetAlpha(foregroundAlpha - (deltaTime /fadeInOutTime));
		m_bDrawText = true;
	}
}

void 
GameEndUI::Draw(Renderer& renderer)
{
	m_pBackground->Draw(renderer, screenOrigin, 0.f, DrawSpace::SCREEN_SPACE);

	if (m_bDrawText)
	{
		Vector2 gameOverTextPos = { renderer.GetWidth() / 2.f, 200.f };
		m_pGameOverText->Draw(renderer, gameOverTextPos, 0.f, DrawSpace::SCREEN_SPACE);

		Vector2 roundTextPos = { renderer.GetWidth() / 2.f, 300.f };
		renderer.DrawDynamicText(m_sRoundText, FontSize::MEDIUM, roundTextPos, { 255, 255, 255, 255 }, SpriteOrigin::CENTER);

		Vector2 timeTextPos = { renderer.GetWidth() / 2.f, 500.f };
		renderer.DrawDynamicText(m_sTimeText, FontSize::SMALL, timeTextPos, { 255, 255, 255, 255 }, SpriteOrigin::CENTER);

		Vector2 killsTextPos = { renderer.GetWidth() / 2.f, 525.f };
		renderer.DrawDynamicText(m_sKillsText, FontSize::SMALL, killsTextPos, { 255, 255, 255, 255 }, SpriteOrigin::CENTER);

		Vector2 actionTextPos = { renderer.GetWidth() / 2.f, 750.f };
		m_pActionText->Draw(renderer, actionTextPos, 0.f, DrawSpace::SCREEN_SPACE);
	}

	m_pForeground->Draw(renderer, screenOrigin, 0.f, DrawSpace::SCREEN_SPACE);
}

bool
GameEndUI::IsLoaded() const
{
	return m_bIsLoaded;
}