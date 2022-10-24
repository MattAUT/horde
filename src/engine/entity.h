#ifndef __ENTITY_H__
#define __ENTITY_H__

class Renderer;
class Input;
class Node;
class SpriteSheetAnimation;
class SmokeEmitter;

namespace FMOD
{
	class System;
	class Sound;
	class Channel;
}

#include "id.h"
#include <vector2.h>
#include <string>
#include <tuple>
#include <map>

#include <game/wall.h>

class Entity
{
	// Methods
public:
	virtual ~Entity();
	// Generates any static assets revelant to the specific entity. Parent
	// method should be called before child method, to ensure entities are
	// not re-initalised
	virtual bool Initialise(Renderer& renderer, FMOD::System& fmodSystem);
	virtual void Process(float deltaTime, Input& input) = 0;
	virtual void Draw(Renderer& renderer) = 0;
#ifdef _DEBUG
	virtual void DebugDraw() = 0;
#endif // _DEBUG

	bool IsColliding(Vector2 position, float radius);
	bool IsLoaded() const;
	void Unload();
	virtual Vector2 GetPosition();
	virtual Vector2 GetVelocity();
	ID GetId() const;
	std::string GetIdAsString();

protected:
	Entity();

	float& GetCollisionRadius();

private:
	Entity(const Entity& e);
	Entity& operator=(const Entity& e) {}

	// Data:
public:
protected:
	ID m_id;
	Vector2 m_position;
	Vector2 m_velocity;
	float m_fCollisionRadius;
	bool m_bInitalised;
	bool m_bIsLoaded;

	Sprite* m_pSprite;
	SpriteSheetAnimation* m_pDeathSprite;

	FMOD::System* m_pFMODsystem;

	std::tuple<unsigned int, Wall*>* m_pGeometry;
	std::tuple<unsigned int, Node*>* m_pPathNodes;
	std::map<WALL::Direction, bool> m_collisionState;
	float m_fSpeed;
	float m_fAngle;
	float m_fHealth;

	FMOD::Sound* m_pHitSound;
	FMOD::Channel* m_pHitSoundChannel;
	FMOD::Sound* m_pDeathSound;
	FMOD::Channel* m_pDeathSoundChannel;
	FMOD::Sound* m_pWalkSound;
	FMOD::Channel* m_pWalkSoundChannel;

	SmokeEmitter* m_pSmokeEmitter;

private:
};

#endif // __ENTITY_H__