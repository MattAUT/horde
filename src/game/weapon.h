#ifndef __WEAPON_H__
#define __WEAPON_H__

class Sprite;
class SpriteSheetAnimation;
class Renderer;
class Enemy;
class Wall;
class Input;

namespace FMOD
{
	class System;
	class Sound;
	class Channel;
}

#include <vector2.h>
#include <tuple>

struct Shot
{
	Vector2 position;
	Vector2 velocity;
	float angle;
	float lifeTime;
	bool active = false;
};

class Weapon
{
public:
	Weapon();
	virtual ~Weapon();
	virtual bool Initialise(Renderer& renderer, FMOD::System& fmodSystem) = 0;
	void Setup(Enemy* enemies, std::tuple<unsigned int, Wall*>* geometry);
	virtual void Process(float deltaTime, Input& input);
	virtual void Draw(Renderer& renderer) = 0;
#ifdef _DEBUG
	virtual void DebugDraw() = 0;
#endif // _DEBUG
	virtual Sprite* GetDrawSprite() const;
	virtual Sprite* GetIconSprite() const;
	virtual void Shoot(Vector2 playerPosition, float angle) = 0;
	float GetHeat() const;
	bool IsOverheated() const;

protected:
private:

public:
	static float sm_fMaxHeat;

protected:
	Sprite* m_pSprite;
	Sprite* m_pIconSprite;
	Enemy* m_pEnemies;
	std::tuple<unsigned int, Wall*>* m_pGeometry;

	FMOD::System* m_pFMODsystem;
	FMOD::Sound* m_pShotSound;
	FMOD::Channel* m_pShotSoundChannel;

	float m_fHeat;
	float m_fCooldownSpeed;
	bool m_bOverheated;

private:

};

#endif // __WEAPON_H__