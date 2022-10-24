#include "shotgun.h"

#include <engine/renderer.h>
#include <engine/spritesheetanimation.h>
#ifdef _DEBUG
#include <imgui/imgui_impl_sdl.h>
#endif // _DEBUG

#include <fmod.hpp>

#include "scenelevel.h"

const float flashDrawDistance = 3.2f;
const float damageAmount = 40.f;
const float heatValue = 30.f;

Shotgun::Shotgun()
{
	m_fCooldownSpeed = 40.f;
}

Shotgun::~Shotgun()
{
	delete m_pShotgunFlash;
	m_pShotgunFlash = nullptr;
}

bool 
Shotgun::Initialise(Renderer& renderer, FMOD::System& fmodSystem)
{
	m_pSprite = renderer.CreateSprite("assets\\sprites\\weapon\\shotgun.png", SpriteOrigin::BOTTOM_CENTER);
	m_pSprite->SetScale(1.4f);
	m_pIconSprite = renderer.CreateSprite("assets\\sprites\\ui\\shotgun-icon.png", SpriteOrigin::CENTER);
	m_pIconSprite->SetScale(3.f);

	m_pShotgunFlash = renderer.CreateAnimatedSprite("assets\\sprites\\weapon\\shotgun_flash.png");
	m_pShotgunFlash->SetupFrames(48, 48, SpriteOrigin::CENTER);
	m_pShotgunFlash->SetFrameDuration(0.025f);
	m_pShotgunFlash->SetScale(3.f);

	m_pFMODsystem = &fmodSystem;
	fmodSystem.createSound("assets\\sounds\\shotgun.wav", FMOD_DEFAULT, NULL, &m_pShotSound);
	return true;
}

void
Shotgun::Process(float deltaTime, Input& input)
{
	Weapon::Process(deltaTime ,input);
	m_pShotgunFlash->Process(deltaTime);
}

void 
Shotgun::Draw(Renderer& renderer)
{
	if (m_pShotgunFlash->IsAnimating())
	{
		m_pShotgunFlash->Draw(renderer, m_drawPosition, m_fDrawAngle);
	}
}


#ifdef _DEBUG
void
Shotgun::DebugDraw()
{
	ImGui::Spacing();
	ImGui::Text("Shotgun Weapon Stats");
	ImGui::Text("Heat Level: %f", m_fHeat);
}
#endif // _DEBUG

void 
Shotgun::Shoot(Vector2 playerPosition, float angle)
{
	if (m_fHeat < sm_fMaxHeat && !m_bOverheated)
	{
		m_pFMODsystem->playSound(m_pShotSound, NULL, false, &m_pShotSoundChannel);
		m_pShotSoundChannel->setVolume(0.5f);

		m_fHeat += heatValue;

		if (m_fHeat > sm_fMaxHeat)
		{
			m_bOverheated = true;
			m_fHeat = sm_fMaxHeat;
		}

		m_pShotgunFlash->Restart();
		m_pShotgunFlash->Animate();

		m_drawPosition = playerPosition + Vector2(angle + static_cast<float>(M_PI) / 2.f, flashDrawDistance, POLAR);
		m_fDrawAngle = angle;

		for (int enemyIndex = 0; enemyIndex < SceneLevel::sm_enemyLoadCount; ++enemyIndex)
		{
			if (m_pEnemies[enemyIndex].IsLoaded())
			{
				float distance = (m_pEnemies[enemyIndex].GetPosition() - m_drawPosition).Length();
				if (distance < flashDrawDistance)
				{
					m_pEnemies[enemyIndex].Damage(damageAmount);
				}
			}
		}
	}
}