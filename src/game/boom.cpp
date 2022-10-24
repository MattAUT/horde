#include "boom.h"

#include <engine/renderer.h>
#include <engine/spritesheetanimation.h>
#ifdef _DEBUG
#include <imgui/imgui_impl_sdl.h>
#endif // _DEBUG

#include <fmod.hpp>

#include "scenelevel.h"

const float shotStartDistance = 1.f;
const float shotMaxLifeTime = 10.f;
const float shotVelocity = 7.5f;
const float explosionRadius = 6.f;
const float damageAmount = 100.f;
const float heatValue = 60.f;

Boom::Boom()
{
	m_fCooldownSpeed = 10.f;
}

Boom::~Boom()
{
	delete m_pShotSprite;
	delete m_pExplosionSprite;

	m_pShotSprite = nullptr;
	m_pExplosionSprite = nullptr;
}

bool 
Boom::Initialise(Renderer& renderer, FMOD::System& fmodSystem)
{
	m_pSprite = renderer.CreateSprite("assets\\sprites\\weapon\\boom.png", SpriteOrigin::BOTTOM_CENTER);
	m_pSprite->SetScale(1.4f);
	m_pIconSprite = renderer.CreateSprite("assets\\sprites\\ui\\boom-icon.png", SpriteOrigin::CENTER);
	m_pIconSprite->SetScale(3.f);

	m_pShotSprite = renderer.CreateAnimatedSprite("assets\\sprites\\weapon\\boom-shot.png");
	m_pShotSprite->SetupFrames(32, 32, SpriteOrigin::CENTER);
	m_pShotSprite->SetFrameDuration(0.05f);
	m_pShotSprite->SetLooping(true);
	m_pShotSprite->Animate();

	m_pExplosionSprite = renderer.CreateAnimatedSprite("assets\\sprites\\boom.png");
	m_pExplosionSprite->SetupFrames(33, 33, SpriteOrigin::CENTER);
	m_pExplosionSprite->SetFrameDuration(0.05f);
	m_pExplosionSprite->SetScale(explosionRadius);

	m_pFMODsystem = &fmodSystem;
	fmodSystem.createSound("assets\\sounds\\shotgun.wav", FMOD_DEFAULT, NULL, &m_pShotSound);
	fmodSystem.createSound("assets\\sounds\\boom.wav", FMOD_DEFAULT, NULL, &m_BoomSound);

	return true;
}

void
Boom::Process(float deltaTime, Input& input)
{
	Weapon::Process(deltaTime ,input);
	m_pShotSprite->Process(deltaTime);
	m_pExplosionSprite->Process(deltaTime);
	
	for (unsigned int shotIndex = 0; shotIndex < Boom::sm_uiLoadedShots; ++shotIndex)
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
					Explode(m_shots[shotIndex].position);
				}
			}

			for (unsigned int wallIndex = 0; wallIndex < std::get<0>(*m_pGeometry); ++wallIndex)
			{
				if (std::get<0>(std::get<1>(*m_pGeometry)[wallIndex].IsColliding(m_shots[shotIndex].position, 0.4f)))
				{
					m_shots[shotIndex].active = false;
					Explode(m_shots[shotIndex].position);
				}
			}
		}
	}
}

void 
Boom::Draw(Renderer& renderer)
{
	for (unsigned int i = 0; i < Boom::sm_uiLoadedShots; ++i)
	{
		if (m_shots[i].active)
		{
			m_pShotSprite->Draw(renderer, m_shots[i].position, m_shots[i].angle);
		}
	}

	if (m_pExplosionSprite->IsAnimating())
	{
		m_pExplosionSprite->Draw(renderer, m_explosionDrawPosition, 0.f);
	}
}


#ifdef _DEBUG
void
Boom::DebugDraw()
{
	unsigned int activeShots = 0;
	for (unsigned int i = 0; i < Boom::sm_uiLoadedShots; ++i)
	{
		if (m_shots[i].active)
		{
			++activeShots;
		}
	}

	ImGui::Spacing();
	ImGui::Text("Boom Weapon Stats");
	ImGui::Text("Heat Level: %f", m_fHeat);
	ImGui::Text("Active Shots: %d", activeShots);
}
#endif // _DEBUG

void 
Boom::Shoot(Vector2 playerPosition, float angle)
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

		for (unsigned int i = 0; i < Boom::sm_uiLoadedShots; ++i)
		{
			if (!m_shots[i].active)
			{
				m_shots[i].position = playerPosition + Vector2(angle + static_cast<float>(M_PI) / 2.f, shotStartDistance, POLAR);;
				m_shots[i].angle = angle;
				m_shots[i].velocity = Vector2(angle + static_cast<float>(M_PI) / 2.f, shotVelocity, POLAR);
				m_shots[i].active = true;
				m_shots[i].lifeTime = 0.f;
				return;
			}
		}
	}
}

void
Boom::Explode(Vector2 position)
{
	m_pFMODsystem->playSound(m_BoomSound, NULL, false, &m_BoomSoundChannel);
	m_BoomSoundChannel->setVolume(0.5f);
	m_explosionDrawPosition = position;
	m_pExplosionSprite->Restart();
	m_pExplosionSprite->Animate();
	for (int enemyIndex = 0; enemyIndex < SceneLevel::sm_enemyLoadCount; ++enemyIndex)
	{
		if (m_pEnemies[enemyIndex].IsLoaded())
		{
			float distance = (m_pEnemies[enemyIndex].GetPosition() - position).Length();
			if (distance < explosionRadius)
			{
				m_pEnemies[enemyIndex].Damage(damageAmount);
			}
		}
	}
}