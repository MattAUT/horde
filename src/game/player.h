#ifndef __PLAYER_H__
#define __PLAYER_H__

class PlayerUI;
class Sprite;
class SpriteSheetAnimation;
class Enemy;
class Weapon;
class Wall;

#include <engine/entity.h>
#include <map>
#include <tuple>

#include "wall.h"

class Player : public Entity
{
public:
	Player();
	virtual ~Player();
	virtual bool Initialise(Renderer& renderer, FMOD::System& fmodSystem) override;
	void Setup(Enemy* enemies, std::tuple<unsigned int, Wall*>& geometry, std::tuple<unsigned int, Node*>& nodes);
	virtual void Process(float deltaTime, Input& input) override;
	virtual void Draw(Renderer& renderer) override;
	void Reset();
#ifdef _DEBUG
	virtual void DebugDraw() override;
#endif // _DEBUG
	void Shoot();
	void Damage(float amount, bool bypass = false);
	void Heal(float amount);
	void IncrementKillCount();
	void ResetKillCount(unsigned int count = 0);
	unsigned int GetKillCount() const;
	unsigned int GetKillCountTotal() const;

	Weapon& GetActiveWeapon() const;
	float GetHealth() const;
	Node* GetClosestNode();
	bool IsDead() const;
protected:
	void SetCollisionState(WALL::Direction direction);
	void DoCollision();

private:
	Player(const Player& p);
	Player& operator=(const Player& p) {}

public:
protected:
	PlayerUI* m_pPlayerUI;
	Enemy* m_pEnemies;
	Weapon** m_pWeapons;
	int m_iActiveWeaponIndex;

	float m_fInvTime;
	bool m_bIsDead;

	Vector2 m_movementInput;
	Vector2 m_rotationInput;
	static int sm_iWeaponCount;
	unsigned int m_uiKillCount;
	unsigned int m_uiKillCountTotal;

	Node* m_pActiveNode;
private:
};

#endif // __PALYER_H__