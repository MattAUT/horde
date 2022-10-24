#ifndef __SCENEMANAGER_H__
#define __SCENEMANAGER_H__

class Game;
class Renderer;
class Camera;
class Scene;

namespace FMOD
{
	class System;
}

#include <vector>

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	bool Initialise(Game& game, Scene** scenes, Camera& camera, Renderer& renderer, FMOD::System& fmodSystem);
	bool SwitchScenes(unsigned int index);
	bool LoadLevel(unsigned int levelIndex);

	Scene* GetActiveScene();
	void Exit();

protected:
private:
	SceneManager(const SceneManager& s);
	SceneManager& operator=(const SceneManager& s) {}

public:
	static const unsigned int sm_uiSceneCount = 3;
protected:
	Scene* m_pScenes[sm_uiSceneCount];
	unsigned int m_iActiveScene;

	Game* m_pGame;
	Renderer* m_pRenderer;
	Camera* m_pCamera;
	FMOD::System* m_pFMODsystem;
private:
};

#endif // __SCENEMANAGER_H__