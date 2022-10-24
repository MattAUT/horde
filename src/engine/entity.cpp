#include "entity.h"

#include "renderer.h"
#include "spritesheetanimation.h"
#include "logmanager.h"

#include <cstdio>
#include <limits>
#include <utils.h>
#include <fmod.hpp>

Entity::Entity()
	: m_position{ 0, 0 }
	, m_velocity{ 0, 0 }
	, m_bInitalised(false)
	, m_bIsLoaded(false)
{
	m_id = ID();
	LogManager::GetInstance().Log("Entity Generated with ID: " + m_id.AsString());
}

Entity::~Entity()
{
	delete m_pDeathSprite;
	m_pDeathSprite = nullptr;
}

bool 
Entity::Initialise(Renderer& renderer, FMOD::System& fmodSystem)
{
	if (m_bInitalised)
	{
		std::string message = "Attemped to re-initalise entity {ID:" + m_id.AsString() + "}";
		LogManager::GetInstance().Error(message);
		return false;
	}

	m_pFMODsystem = &fmodSystem;
	fmodSystem.createSound("assets\\sounds\\hit.wav", FMOD_DEFAULT, NULL, &m_pHitSound);
	fmodSystem.createSound("assets\\sounds\\death.wav", FMOD_DEFAULT, NULL, &m_pDeathSound);
	fmodSystem.createSound("assets\\sounds\\walk.wav", FMOD_DEFAULT, NULL, &m_pWalkSound);

	m_pDeathSprite = renderer.CreateAnimatedSprite("assets\\sprites\\boom.png");
	m_pDeathSprite->SetupFrames(33, 33, SpriteOrigin::CENTER);
	m_pDeathSprite->SetScale(2.f);
	m_pDeathSprite->SetFrameDuration(0.05f);

	return true;
}

bool
Entity::IsColliding(Vector2 position, float radius)
{
	Vector2 diff = m_position - position;
	float distance = diff.Length();

	return distance < m_fCollisionRadius + radius;
}

bool
Entity::IsLoaded() const
{
	return m_bIsLoaded;
}

void 
Entity::Unload()
{
	m_bIsLoaded = false;
}

Vector2
Entity::GetPosition()
{
	return m_position;
}

Vector2
Entity::GetVelocity()
{
	return m_velocity;
}

ID
Entity::GetId() const
{
	return m_id;
}

std::string
Entity::GetIdAsString()
{
	return m_id.AsString();
}

float&
Entity::GetCollisionRadius()
{
	return m_fCollisionRadius;
}