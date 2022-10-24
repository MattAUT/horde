#ifndef __SCENELEVEL_H__
#define __SCENELEVEL_H__

#include <engine/scene.h>
#include <vector>

#include "enemy.h"
#include "round.h"
#include <tuple>

// Forward Declarations
class SceneManager;
class Input;
class Sprite;
class Player;
class Wall;
class Node;
class GameEndUI;
class PauseUI;

namespace FMOD
{
	class System;
	class Channel;
	class Sound;
}

enum State
{
	IN_ROUND,
	ROUND_TRANSITION,
	PAUSE,
	GAME_OVER,
};

class SceneLevel : public Scene
{
	// Member Methods
public:
	SceneLevel();
	virtual ~SceneLevel();

	void SetLevel(unsigned int level);
	virtual bool Initialise(Renderer& renderer, Camera& camera, SceneManager& sceneManager, FMOD::System& fmodSystem) override;
	virtual void Deinitialise() override;
	virtual void Process(float deltaTime, Input& input) override;
	virtual void Reset() override;
	virtual void Draw(Renderer& renderer) override;
#ifdef _DEBUG
	virtual void DebugDraw(Renderer& renderer) override;
#endif // _DEBUG
	void TogglePause();
	void Exit();

protected:
private:
	SceneLevel(const SceneLevel& s);
	SceneLevel& operator=(const SceneLevel& s) {}

	// Member Data
public:
	static const int sm_enemyLoadCount = 24;

protected:
	unsigned int m_uiLevel;
	Player* m_pPlayer;
	Enemy* m_enemies; 
	std::tuple<unsigned int, Wall*> m_geometry;
	std::tuple<unsigned int, Node*> m_pathNodes;
	std::tuple<unsigned int, Vector2*> m_spawnLocations;

	Round m_round;
	unsigned int m_uiEnemiesSpawned;
	float m_fTotalTime;

	State m_state;
	State m_previousState;
	GameEndUI* m_pGameEndUI;
	PauseUI* m_pPauseUI;

	FMOD::Sound* m_pWalkSound;
	FMOD::Channel* m_pWalkSoundChannel;
	FMOD::Sound* m_pRoundChangeSound;
	FMOD::Channel* m_pRoundChangeSoundChannel;

#ifdef _DEBUG
	Sprite* m_pNodeSprite;
	Sprite* m_pSpawnerSprite;
#endif // _DEBUG
private:

};

#endif // __SCENEGAME_H__

