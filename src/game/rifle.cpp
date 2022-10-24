#include "rifle.h"

#include <engine/renderer.h>
#include <engine/spritesheetanimation.h>
#ifdef _DEBUG
#include <imgui/imgui_impl_sdl.h>
#endif // _DEBUG

#include <tuple>
#include <fmod.hpp>

#include "enemy.h"
#include "scenelevel.h"

const float shotStartDistance = 1.;
const float shotMaxLifeTime = 1.f;
const float shotVelocity = 60.f;
const float heatValue = 15.f;
const float damageAmount = 40.f;


Rifle::Rifle()
{
	m_fCooldownSpeed = 60.f;
}

Rifle::~Rifle()
{
	delete m_pShotSprite;
	m_pShotSprite = nullptr;
}

bool 
Rifle::Initialise(Renderer& renderer, FMOD::System& fmodSystem)
{
	m_pSprite = renderer.CreateSprite("assets\\sprites\\weapon\\rifle.png", SpriteOrigin::BOTTOM_CENTER);
	m_pSprite->SetScale(1.4f);
	m_pShotSprite = renderer.CreateSprite("assets\\sprites\\weapon\\shot.png", SpriteOrigin::CENTER);
	
	m_pIconSprite = renderer.CreateSprite("assets\\sprites\\ui\\rifle-icon.png", SpriteOrigin::CENTER);
	m_pIconSprite->SetScale(3.f);

	m_pFMODsystem = &fmodSystem;
	fmodSystem.createSound("assets\\sounds\\rifle.wav", FMOD_DEFAULT, NULL, &m_pShotSound);

	return true;
}

void
Rifle::Process(float deltaTime, Input& input)
{
	Weapon::Process(deltaTime, input);

	for (unsigned int shotIndex = 0; shotIndex < Rifle::sm_uiLoadedShots; ++shotIndex)
	{
		if (m_shots[shotIndex].active)
		{
			m_shots[shotIndex].lifeTime += deltaTime;
			if (m_shots[shotIndex].lifeTime > shotMaxLifeTime)
			{
				m_shots[shotIndex].active = false;
			}

			m_shots[shotIndex].position += m_shots[shotIndex].velocity * deltaTime;

			for (unsigned int enemyIndex = 0; enemyIndex < SceneLevel::sm_enemyLoadCount; ++enemyIndex)
			{
				if (m_pEnemies[enemyIndex].IsLoaded() && m_pEnemies[enemyIndex].IsColliding(m_shots[shotIndex].position, 0.4f))
				{
					m_shots[shotIndex].active = false;
					m_pEnemies[enemyIndex].Damage(damageAmount);
				}
			}

			for (unsigned int wallIndex = 0; wallIndex < std::get<0>(*m_pGeometry); ++wallIndex)
			{
				if (std::get<0>(std::get<1>(*m_pGeometry)[wallIndex].IsColliding(m_shots[shotIndex].position, 0.4f)))
				{
					m_shots[shotIndex].active = false;
				}
			}
		}
	}
}

void 
Rifle::Draw(Renderer& renderer)
{
	for (unsigned int i = 0; i < Rifle::sm_uiLoadedShots; ++i)
	{
		if (m_shots[i].active)
		{
			m_pShotSprite->Draw(renderer, m_shots[i].position, m_shots[i].angle);
		}
	}
}

#ifdef _DEBUG
void 
Rifle::DebugDraw()
{
	unsigned int activeShots = 0;
	for (unsigned int i = 0; i < Rifle::sm_uiLoadedShots; ++i)
	{
		if (m_shots[i].active)
		{
			++activeShots;
		}
	}

	ImGui::Spacing();
	ImGui::Text("Rifle Weapon Stats");
	ImGui::Text("Heat Level: %f", m_fHeat);
	ImGui::Text("Active Shots: %d", activeShots);
}
#endif // _DEBUG

void 
Rifle::Shoot(Vector2 position, float angle)
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

		for (unsigned int i = 0; i < Rifle::sm_uiLoadedShots; ++i)
		{
			if (!m_shots[i].active)
			{
				m_shots[i].position = position + Vector2(angle + static_cast<float>(M_PI) / 2.f, shotStartDistance, POLAR);;
				m_shots[i].angle = angle;
				m_shots[i].velocity = Vector2(angle + static_cast<float>(M_PI) / 2.f, shotVelocity, POLAR);
				m_shots[i].active = true;
				m_shots[i].lifeTime = 0.f;
				return;
			}
		}
	}
}