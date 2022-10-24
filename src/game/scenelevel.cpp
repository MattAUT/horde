// COMP710 GP Framework 2022

#include "scenelevel.h"

#include <engine/renderer.h>
#include <engine/sprite.h>
#include <engine/scenemanager.h>
#include <engine/camera.h>
#include <engine/input.h>
#include <engine/xinputcontroller.h>

#include <cassert>
#include <box2d/box2d.h>
#include <utils.h>
#include <logmanager.h>
#include <ini.h>
#include <cmath>
#include <string>
#ifdef _DEBUG
#include <imgui/imgui_impl_sdl.h>
#endif // _DEBUG
#include <fmod.hpp>

#include "player.h"
#include "enemy.h"
#include "wall.h"
#include "node.h"
#include "gameendui.h"
#include "pauseui.h"

const float roundTransitionTime = 5.f;

SceneLevel::SceneLevel()
	: m_uiEnemiesSpawned(0)
	, m_state(State::ROUND_TRANSITION)
	, m_fTotalTime(0.f)
{
#ifdef _DEBUG
	dm_name = "Level Scene";
#endif // _DEBUG
}

SceneLevel::~SceneLevel()
{
	if (m_bIsLoaded)
	{
		Deinitialise();
	}	
}

void
SceneLevel::SetLevel(unsigned int level)
{
	m_uiLevel = level;
}

bool 
SceneLevel::Initialise(Renderer& renderer, Camera& camera, SceneManager& sceneManager, FMOD::System& fmodSystem)
{
	m_uiEnemiesSpawned = 0;
	m_state = State::ROUND_TRANSITION;
	m_fTotalTime = 0.f;
	m_round = Round();

	INI::Settings levelData;
	char levelFilePath[32];
	sprintf_s(levelFilePath, "assets\\levels\\%d.lvl", m_uiLevel);
	levelData.LoadFile(levelFilePath);

	std::get<0>(m_geometry) = levelData.GetInt("geometry", "count");
	std::get<1>(m_geometry) = new Wall[std::get<0>(m_geometry)];

	for (unsigned int wallIndex = 0; wallIndex < std::get<0>(m_geometry); ++wallIndex)
	{
		std::get<1>(m_geometry)[wallIndex].Initialise(renderer, levelData, wallIndex);
	}

	std::get<0>(m_pathNodes) = levelData.GetInt("path_nodes", "count");
	std::get<1>(m_pathNodes) = new Node[std::get<0>(m_pathNodes)];

	for (unsigned int nodeIndex = 0; nodeIndex < std::get<0>(m_pathNodes); ++nodeIndex)
	{
		std::get<1>(m_pathNodes)[nodeIndex].Initialise(renderer, levelData, nodeIndex);
	}

	for (unsigned int nodeIndex = 0; nodeIndex < std::get<0>(m_pathNodes); ++nodeIndex)
	{
		std::get<1>(m_pathNodes)[nodeIndex].Setup(std::get<1>(m_pathNodes));
	}

	fmodSystem.createSound("assets\\sounds\\walk.wav", FMOD_DEFAULT, NULL, &m_pWalkSound);
	fmodSystem.createSound("assets\\sounds\\round-change.wav", FMOD_DEFAULT, NULL, &m_pRoundChangeSound);

	std::get<0>(m_spawnLocations) = levelData.GetInt("spawns", "count");
	std::get<1>(m_spawnLocations) = new Vector2[std::get<0>(m_spawnLocations)];
	for (unsigned int spawnIndex = 0; spawnIndex < std::get<0>(m_spawnLocations); ++spawnIndex)
	{
		char spawnXpos[16];
		char spawnYpos[16];

		sprintf_s(spawnXpos, "%dx", spawnIndex);
		sprintf_s(spawnYpos, "%dy", spawnIndex);

		std::get<1>(m_spawnLocations)[spawnIndex] =
		{ 
			levelData.GetFloat("spawns", spawnXpos), 
			levelData.GetFloat("spawns", spawnYpos) 
		};
	}

	levelData.Unload();

	m_pPlayer = new Player();
#ifdef _DEBUG
	AddEntityToDebugQueue(m_pPlayer);
#endif // _DEBUG

	m_enemies = new Enemy[sm_enemyLoadCount];
	for (int i = 0; i < sm_enemyLoadCount; ++i)
	{
		m_enemies[i].Initialise(renderer, fmodSystem);
		m_enemies[i].Setup(*m_pPlayer, m_geometry, m_pathNodes);
#ifdef _DEBUG
		AddEntityToDebugQueue(&m_enemies[i]);
#endif // _DEBUG
	}

	m_pPlayer->Initialise(renderer, fmodSystem);
	m_pPlayer->Setup(m_enemies, m_geometry, m_pathNodes);

#ifdef _DEBUG
	m_pNodeSprite = renderer.CreateSprite("assets\\sprites\\ui\\bar.png", SpriteOrigin::CENTER);
	m_pNodeSprite->SetBlueTint(0.f);
	m_pNodeSprite->SetRedTint(0.f);
	m_pSpawnerSprite = renderer.CreateSprite("assets\\sprites\\ui\\bar.png", SpriteOrigin::CENTER);
	m_pSpawnerSprite->SetBlueTint(0.f);
	m_pSpawnerSprite->SetGreenTint(0.f);
#endif // _DEBUG

	m_pGameEndUI = new GameEndUI();
	m_pGameEndUI->Initialise(renderer);

	m_pPauseUI = new PauseUI();
	m_pPauseUI->Initialise(renderer, *this, fmodSystem);

	renderer.SetClearColour(10, 10, 10);

	return Scene::Initialise(renderer, camera, sceneManager, fmodSystem);
}

void
SceneLevel::Deinitialise()
{
	delete m_pPlayer;
	delete[] std::get<1>(m_geometry);
	delete[] std::get<1>(m_pathNodes);
	delete[] std::get<1>(m_spawnLocations);

	m_pPlayer = nullptr;
	std::get<1>(m_geometry) = nullptr;
	std::get<1>(m_pathNodes) = nullptr;
	std::get<1>(m_spawnLocations) = nullptr;

	delete[] m_enemies;
	m_enemies = nullptr;

#ifdef _DEBUG
	delete m_pNodeSprite;
	delete m_pSpawnerSprite;

	m_pNodeSprite = nullptr;
	m_pSpawnerSprite = nullptr;
#endif // _DEBUG

	delete m_pGameEndUI;
	delete m_pPauseUI;
	m_pGameEndUI = nullptr;
	m_pPauseUI = nullptr;

	Scene::Deinitialise();
}

void 
SceneLevel::Process(float deltaTime, Input& input)
{
	m_pCamera->SetPosition(m_pPlayer->GetPosition());
	if (m_pPlayer->IsDead())
	{
		if (!m_pGameEndUI->IsLoaded())
		{
			m_pGameEndUI->Setup({m_round.GetCurrentRound(), m_pPlayer->GetKillCountTotal(), m_fTotalTime});
		}
		m_state = State::GAME_OVER;
		m_pGameEndUI->Process(deltaTime);
	}
	
	static XInputController* controller = input.GetController(0);
	static float timeUntilTransition = roundTransitionTime;

	if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_START) == BS_PRESSED)
	{
		if (m_state == State::GAME_OVER)
		{
			timeUntilTransition = roundTransitionTime;
			m_pSceneManager->SwitchScenes(1);
			return;
		}

		TogglePause();
	}

	if (m_state == State::PAUSE)
	{
		m_pPauseUI->Process(deltaTime, input);
		deltaTime = 0.f;
	}

	m_fTotalTime += deltaTime;

	if (m_state == State::ROUND_TRANSITION)
	{
		timeUntilTransition -= deltaTime;
		if (timeUntilTransition <= 0.f)
		{
			m_state = State::IN_ROUND;
			timeUntilTransition = roundTransitionTime;
		}
	}
	
	Enemy* lastProcessedEnemy = nullptr;
	for (int i = 0; i < sm_enemyLoadCount; ++i)
	{
		if (m_enemies[i].IsLoaded())
		{
			m_enemies[i].Process(deltaTime, input);
			lastProcessedEnemy = &m_enemies[i];
		}
	}

	static float timeUntilWalkSound = 0.f;
	if (lastProcessedEnemy != nullptr)
	{
		timeUntilWalkSound -= deltaTime;
		if (timeUntilWalkSound <= 0.f && m_state != State::GAME_OVER)
		{
			m_pFMODsystem->playSound(m_pWalkSound, NULL, false, &m_pWalkSoundChannel);
			m_pWalkSoundChannel->setVolume(0.4f);
			timeUntilWalkSound = Enemy::sm_fWalkSoundDelay;
		}
	}

	static float timeUntilNextSpawn = m_round.GetSpawnDelay();
	timeUntilNextSpawn -= deltaTime;

	if (m_state == State::IN_ROUND)
	{
		if (m_uiEnemiesSpawned < m_round.GetSpawnCount() && timeUntilNextSpawn <= 0.f)
		{
			for (int i = 0; i < sm_enemyLoadCount; ++i)
			{
				if (!m_enemies[i].IsLoaded())
				{
					unsigned int spawnerIndex = GetRandom(0, std::get<0>(m_spawnLocations) - 1);
					
					char str[8];
					sprintf_s(str, "%d", i);
					LogManager::GetInstance().Log(str);
					m_enemies[i].Reinitialise(std::get<1>(m_spawnLocations)[spawnerIndex], m_round.GetSpawnHealth(), m_pPlayer->GetClosestNode());
					timeUntilNextSpawn = m_round.GetSpawnDelay();
					++m_uiEnemiesSpawned;
					return;
				}
			}
		}

		if (m_round.GetSpawnCount() == m_pPlayer->GetKillCount())
		{
			m_pFMODsystem->playSound(m_pRoundChangeSound, NULL, false, &m_pRoundChangeSoundChannel);
			m_state = State::ROUND_TRANSITION;
			
			m_uiEnemiesSpawned = 0;
			m_pPlayer->ResetKillCount();
			m_round.Increment();
		}
	}
	
	m_pPlayer->Process(deltaTime, input);
}

void 
SceneLevel::Draw(Renderer& renderer)
{
	for (unsigned int i = 0; i < std::get<0>(m_geometry); ++i)
	{
		std::get<1>(m_geometry)[i].Draw(renderer);
	}

	for (int i = 0; i < sm_enemyLoadCount; ++i)
	{
		if (m_enemies[i].IsLoaded())
		{
			m_enemies[i].Draw(renderer);
		}
	}

	m_pPlayer->Draw(renderer);

	// UI
	char roundText[16];
	sprintf_s(roundText, "ROUND: %d", m_round.GetCurrentRound());
	Vector2 roundTextPos = { 20.f, 20.f };
	renderer.DrawDynamicText(roundText, FontSize::MEDIUM, roundTextPos, { 255, 255, 255, 255 }, SpriteOrigin::TOP_LEFT);

	if (m_state == State::PAUSE)
	{
		m_pPauseUI->Draw(renderer);
	}

	if (m_pGameEndUI->IsLoaded())
	{
		m_pGameEndUI->Draw(renderer);
	}
}

void
SceneLevel::Reset()
{
	m_pPlayer->Reset();

	for (int i = 0; i < sm_enemyLoadCount; ++i)
	{
		if (m_enemies[i].IsLoaded())
		{
			m_enemies[i].Reset();
		}
	}
}

#ifdef _DEBUG
void 
SceneLevel::DebugDraw(Renderer& renderer)
{
	Scene::BeginDebugDraw();
	m_pPlayer->DebugDraw();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("Camera Position: %f, %f", m_pCamera->GetPosition().x, m_pCamera->GetPosition().y);
	ImGui::Text("Round: %d", m_round.GetCurrentRound());
	ImGui::Text("Round Spawn Count: %d", m_round.GetSpawnCount());
	ImGui::Text("Round Spawn Delay: %f", m_round.GetSpawnDelay());
	ImGui::Text("Enemies Spawned: %d", m_uiEnemiesSpawned);
	ImGui::End();

	for (unsigned int i = 0; i < std::get<0>(m_pathNodes); ++i)
	{
		m_pNodeSprite->Draw(renderer, std::get<1>(m_pathNodes)[i].GetPosition(), 0.f);
	}

	for (unsigned int i = 0; i < std::get<0>(m_spawnLocations); ++i)
	{
		m_pSpawnerSprite->Draw(renderer, std::get<1>(m_spawnLocations)[i], 0.f);
	}
}
#endif // _DEBUG

void 
SceneLevel::TogglePause()
{
	static State previousState = m_state;
	if (m_state == State::PAUSE)
	{
		m_state = previousState;
		previousState = State::PAUSE;
		return;
	}

	previousState = m_state;
	m_state = State::PAUSE;
}

void
SceneLevel::Exit()
{
	m_pPlayer->Damage(100.f, true);
	
}

