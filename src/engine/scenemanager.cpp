#include "scenemanager.h"

#include "game/game.h"
#include "renderer.h"
#include "scene.h"
#include "logmanager.h"
#include "game/scenelevel.h"

const unsigned int levelSceneIndex = 2;

SceneManager::SceneManager()
	: m_iActiveScene(0)
{

}

SceneManager::~SceneManager()
{
	for (unsigned int i = 0; i < sm_uiSceneCount; ++i)
	{
		delete m_pScenes[i];
		m_pScenes[i] = nullptr;
	}
}

bool
SceneManager::Initialise(Game& game, Scene** scenes, Camera& camera, Renderer& renderer, FMOD::System& fmodSystem)
{
	m_pGame = &game;
	m_pCamera = &camera;
	m_pRenderer = &renderer;
	m_pFMODsystem = &fmodSystem;

	for (unsigned int i = 0; i < sm_uiSceneCount; ++i)
	{
		m_pScenes[i] = scenes[i];
	}

	LogManager::GetInstance().Log("SceneManager Initalised");
	return true;
}

bool
SceneManager::SwitchScenes(unsigned int index)
{
	if (index > sm_uiSceneCount - 1)
	{
		return false;
	}

	if (m_pScenes[m_iActiveScene]->IsLoaded())
	{
		m_pScenes[m_iActiveScene]->Deinitialise();
	}

	if (!m_pScenes[index]->Initialise(*m_pRenderer, *m_pCamera, *this, *m_pFMODsystem))
	{
		LogManager::GetInstance().Error("SceneManager failed to initalise a scene!");
	}

	m_iActiveScene = index;
	return true;
}

bool 
SceneManager::LoadLevel(unsigned int levelIndex)
{
	assert((dynamic_cast<SceneLevel*>(m_pScenes[levelSceneIndex]) != nullptr));
	if (dynamic_cast<SceneLevel*>(m_pScenes[levelSceneIndex]) != nullptr)
	{
		dynamic_cast<SceneLevel*>(m_pScenes[levelSceneIndex])->SetLevel(levelIndex);
		return SwitchScenes(levelSceneIndex);
	}
	
	return false;
}

Scene* 
SceneManager::GetActiveScene()
{
	return m_pScenes[m_iActiveScene];
}

void
SceneManager::Exit()
{
	m_pGame->Quit();
}