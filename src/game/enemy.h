#ifndef __ENEMY_H__
#define __ENEMY_H__

class Sprite;
class Player;
class Node;

#include <stack>
#include <vector>
#include <engine/entity.h>
#include "wall.h"

typedef std::tuple<std::stack<Vector2>, float> Path;

namespace ENEMY
{
	enum State
	{
		ALIVE,
		DEAD,
	};

	enum Pathing
	{
		SEARCHING,
		FOLLOWING,
	};
}

class Enemy : public Entity
{
public:
	Enemy();
	virtual ~Enemy();
	virtual bool Initialise(Renderer& renderer, FMOD::System& fmodSystem) override;
	void Setup(Player& player, std::tuple<unsigned int, Wall*>& geometry, std::tuple<unsigned int, Node*>& nodes);
	bool Reinitialise(Vector2 position, float health, Node* closestNodeToPlayer);
	virtual void Process(float deltaTime, Input& input) override;
	virtual void Draw(Renderer& renderer) override;
	void Reset();
#ifdef _DEBUG
	virtual void DebugDraw() override;
#endif // _DEBUG
	void Damage(float amount);
	float GetCollisionRadius();
	void UpdateTravelPath(Node* closestNodeToPlayer);
	bool IsInLOS(Vector2 position);

protected:
	void SetCollisionState(WALL::Direction direction);
	void SearchForPath(std::vector<Path>& paths, Node* node, Node* prevNode, int currentPathIndex, unsigned int searchDepth, unsigned int maxSearchDepth);

private:
	Enemy(const Enemy& e);
	Enemy& operator=(const Enemy& e) {}

public:
	static float sm_fWalkSoundDelay;

protected:
	Player* m_pPlayer;
	std::stack<Vector2> m_travelPath;
	ENEMY::State m_state;
	ENEMY::Pathing m_pathingState;

	FMOD::Sound* m_pSpawnSound;
	FMOD::Channel* m_pSpawnSoundChannel;

	float m_fLifeTime;
private:
};

#endif // __ENEMY_H__