#include "playerui.h"

#include <engine/renderer.h>
#include <engine/spritesheetanimation.h>
#include <engine/scene.h>

#include "player.h"
#include "weapon.h"

const float heatBarWidthScale = 4.f;
const float healthBarWidthScale = 8.f;

const float overheatBlueTint = 1.f;
const float overheatGreenTint = 0.4f;
const float overheatRedTint = 0.f;

const Vector2 screenOrigin = { 0.f, 0.f };

PlayerUI::PlayerUI()
{

}

PlayerUI::~PlayerUI()
{
	delete m_pOverheatBar;
	delete m_pOverheatBarUnderlay;
	delete m_pHealthBar; 
	delete m_pHealthBarUnderlay;
	delete m_pHealthIcon;
	delete m_pWeaponIcon;
	delete m_pDamageFlash;

	m_pOverheatBar = nullptr;
	m_pOverheatBarUnderlay = nullptr;
	m_pHealthBar = nullptr;
	m_pHealthBarUnderlay = nullptr;
	m_pHealthIcon = nullptr;
	m_pWeaponIcon = nullptr;
	m_pDamageFlash = nullptr;
}

bool
PlayerUI::Initialise(Renderer& renderer, Player& player)
{
	m_pPlayer = &player;
	m_pOverheatBar = renderer.CreateSprite("assets\\sprites\\ui\\bar.png", SpriteOrigin::CENTER);
	m_pOverheatBar->SetRedTint(overheatRedTint);
	m_pOverheatBar->SetGreenTint(overheatGreenTint);

	m_pOverheatBarUnderlay = renderer.CreateSprite("assets\\sprites\\ui\\bar.png", SpriteOrigin::CENTER);
	m_pOverheatBarUnderlay->SetScaleX(Weapon::sm_fMaxHeat / heatBarWidthScale);
	m_pOverheatBarUnderlay->SetAlpha(0.2f);
	m_pOverheatBarUnderlay->SetRedTint(overheatRedTint);
	m_pOverheatBarUnderlay->SetGreenTint(overheatGreenTint);

	m_pHealthBar = renderer.CreateSprite("assets\\sprites\\ui\\bar.png", SpriteOrigin::TOP_RIGHT);
	m_pHealthBar->SetBlueTint(0.f);
	m_pHealthBar->SetGreenTint(0.f);

	m_pHealthBarUnderlay = renderer.CreateSprite("assets\\sprites\\ui\\bar.png", SpriteOrigin::TOP_RIGHT);
	m_pHealthBarUnderlay->SetScaleX(100.f / healthBarWidthScale);
	m_pHealthBarUnderlay->SetAlpha(0.2f);

	m_pHealthIcon = renderer.CreateSprite("assets\\sprites\\ui\\health-icon.png", SpriteOrigin::TOP_LEFT);

	m_pDamageFlash = renderer.CreateAnimatedSprite("assets\\sprites\\ui\\damage-flash.png");
	m_pDamageFlash->SetupFrames(1, 1, SpriteOrigin::TOP_LEFT);
	m_pDamageFlash->SetScale(static_cast<float>(renderer.GetWidth()));
	m_pDamageFlash->SetAlpha(0.6f);
	m_pDamageFlash->SetFrameDuration(0.025f);

	m_pWeaponIcon = renderer.CreateSprite("assets\\sprites\\ui\\weapon-icon.png", SpriteOrigin::CENTER);
	m_pWeaponIcon->SetScale(3.f);
	return true;
}

void
PlayerUI::Process(float deltaTime)
{
	m_pDamageFlash->Process(deltaTime);

	m_pOverheatBar->SetScaleX(m_pPlayer->GetActiveWeapon().GetHeat() / heatBarWidthScale);
	m_pHealthBar->SetScaleX(m_pPlayer->GetHealth() / healthBarWidthScale);

	if (m_pPlayer->GetActiveWeapon().IsOverheated())
	{
		m_pOverheatBar->SetGreenTint(0.f);
		m_pOverheatBar->SetBlueTint(0.f);
		m_pOverheatBar->SetRedTint(1.f);
	}
	else
	{
		m_pOverheatBar->SetRedTint(overheatRedTint);
		m_pOverheatBar->SetGreenTint(overheatGreenTint);
		m_pOverheatBar->SetBlueTint(overheatBlueTint);
	}
}

void 
PlayerUI::Draw(Renderer& renderer)
{
	Vector2 overheatBarPos = { renderer.GetWidth() / 2.f, renderer.GetHeight() - 50.f };
	Vector2 healthBarPos = { renderer.GetWidth() - 80.f, 50.f };

	m_pOverheatBarUnderlay->Draw(renderer, overheatBarPos, 0.f, DrawSpace::SCREEN_SPACE);
	m_pOverheatBar->Draw(renderer, overheatBarPos, 0.f, DrawSpace::SCREEN_SPACE);

	m_pHealthBarUnderlay ->Draw(renderer, healthBarPos, 0.f, DrawSpace::SCREEN_SPACE);
	m_pHealthBar->Draw(renderer, healthBarPos, 0.f, DrawSpace::SCREEN_SPACE);

	m_pHealthIcon->Draw(renderer, healthBarPos, 0.f, DrawSpace::SCREEN_SPACE);

	Vector2 weaponIconPos = { 100.f, renderer.GetHeight() - 75.f };
	m_pWeaponIcon->Draw(renderer, weaponIconPos, 0.f, DrawSpace::SCREEN_SPACE);
	m_pPlayer->GetActiveWeapon().GetIconSprite()->Draw(renderer, weaponIconPos, 0.f, DrawSpace::SCREEN_SPACE);

	m_pDamageFlash->Draw(renderer, screenOrigin, 0.0f, DrawSpace::SCREEN_SPACE);
}

void 
PlayerUI::DoDamageFlash()
{
	m_pDamageFlash->Restart();
	m_pDamageFlash->Animate();
}
