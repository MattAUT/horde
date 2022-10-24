#include "smokeemitter.h"

#include <engine/renderer.h>
#include <engine/sprite.h>
#include <engine/particle.h>

#include <utils.h>

SmokeEmitter::SmokeEmitter()
	: m_fEmitRate(0.01f)
	, m_fTimeUntilNextSpawn(0.f)
	, m_fDeccelerationScalar(0.25f)
	, m_fInitialVelocityScalar(0.5f)
	, m_fAngle(0.f)
	, m_fSpread(static_cast<float>(M_PI))
	, m_fMaxLifespan(0.2f)
	, m_fScale(1.2f)
	, m_position{ 0.f, 0.f }
	, m_bIsActive(false)
{

}

SmokeEmitter::~SmokeEmitter()
{
	delete m_pSharedSprite;
	delete[] m_particles;

	m_pSharedSprite = nullptr;
	m_particles = nullptr;
}

bool 
SmokeEmitter::Initialise(Renderer& renderer, float emitRate, float decceleration, float initalVelocity, float spread, float scale, float lifeSpan)
{
	m_pSharedSprite = renderer.CreateSprite("assets\\sprites\\smoke.png", SpriteOrigin::CENTER);
	m_particles = new Particle[sm_uiTotalParticles];

	for (unsigned int i = 0; i < sm_uiTotalParticles; ++i)
	{
		m_particles[i].Initialise(*m_pSharedSprite);
	}

	m_fEmitRate = emitRate;
	m_fDeccelerationScalar = decceleration;
	m_fInitialVelocityScalar = initalVelocity;
	m_fSpread = spread;
	m_fMaxLifespan = lifeSpan;
	m_fScale = scale;

	return true;
}

void 
SmokeEmitter::Process(float deltaTime)
{
	if (m_bIsActive)
	{
		m_fTimeUntilNextSpawn -= deltaTime;

		if (m_fTimeUntilNextSpawn <= 0.f)
		{
			Spawn();
			m_fTimeUntilNextSpawn = m_fEmitRate;
		}
	}

	for (unsigned int i = 0; i < sm_uiTotalParticles; ++i)
	{
		if (m_particles[i].IsAlive())
		{
			m_particles[i].Process(deltaTime);
		}
	}
}

void 
SmokeEmitter::Draw(Renderer& renderer)
{
	for (unsigned int i = 0; i < sm_uiTotalParticles; ++i)
	{
		if (m_particles[i].IsAlive())
		{
			m_particles[i].Draw(renderer);
		}
	}
}

#ifdef _DEBUG
void 
SmokeEmitter::DebugDraw()
{

}
#endif // _DEBUG

void 
SmokeEmitter::Spawn()
{
	for (unsigned int i = 0; i < sm_uiTotalParticles; ++i)
	{
		if (!m_particles[i].IsAlive())
		{
			float angle = GetRandom_f(m_fAngle - m_fSpread / 2.f, m_fAngle + m_fSpread / 2.f);
			Vector2 velocity = Vector2(angle, m_fInitialVelocityScalar, POLAR);
			Vector2 acceleration = Vector2(angle + static_cast<float>(M_PI), m_fDeccelerationScalar, POLAR);
			m_particles[i].Setup(m_position, velocity, acceleration, m_fMaxLifespan, m_fScale - 0.2f, m_fScale + 0.2f);
			return;
		}		
	}
}

void 
SmokeEmitter::SetPosition(const Vector2& position)
{
	m_position = position;
}

void 
SmokeEmitter::SetAngle(float angle)
{
	m_fAngle = angle + static_cast<float>(M_PI) / 2.f;
}

void
SmokeEmitter::SetActive(bool active)
{
	m_bIsActive = active;
}