#include "particle.h" 

#include "renderer.h" 
#include "sprite.h" 

#include <utils.h>

Particle::Particle()
	: m_bAlive(false)
	, m_fCurrentAge(0.0f)
{

}

Particle::~Particle()
{

}

bool
Particle::Initialise(Sprite& sprite)
{
	m_pSharedSprite = &sprite;
	return true;
}

void
Particle::Setup(Vector2 position, Vector2 velocity, Vector2 acceleration, float lifeSpan, float minScale, float maxScale)
{
	m_postion = position;
	m_velocity = velocity;
	m_acceleration = acceleration;
	m_fMaxLifespan = lifeSpan;
	m_bAlive = true;
	m_fAngle = GetRandom_f(0, 2 * static_cast<float>(M_PI));
	m_fScale = GetRandom_f(minScale, maxScale);
	m_fCurrentAge = 0.f;
}

void
Particle::Process(float deltaTime)
{
	m_fCurrentAge += deltaTime;
	if (m_velocity.Length() > 0.1f)
	{
		m_velocity += m_acceleration * deltaTime;
	}
		
	m_postion += m_velocity * deltaTime;

	if (m_fCurrentAge > m_fMaxLifespan)
	{
		m_bAlive = false;
	}
}

void
Particle::Draw(Renderer& renderer)
{
	float alpha = 0.25f - (m_fCurrentAge / m_fMaxLifespan) / 4.f;
	m_pSharedSprite->SetAlpha(alpha);
	m_pSharedSprite->SetScale(m_fScale);
	m_pSharedSprite->Draw(renderer, m_postion, m_fAngle);
	
}

bool
Particle::IsAlive() const
{
	return m_bAlive;
}