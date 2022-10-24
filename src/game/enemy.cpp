#include "enemy.h"

#include <engine/renderer.h>
#include <engine/sprite.h>
#include <engine/spritesheetanimation.h>
#include <engine/input.h>
#include <engine/xinputcontroller.h>
#include <logmanager.h>
#ifdef _DEBUG
#include <imgui/imgui_impl_sdl.h>
#endif // _DEBUG

#include <utils.h>
#include <fmod.hpp>
#include <limits>

#include "player.h"
#include "smokeemitter.h"
#include "node.h"

float Enemy::sm_fWalkSoundDelay = 0.325f;

const float damageAmount = 25.f;
const float healAmount = 0.1f;
const float walkSoundDelay = 0.325f;
const float beginDamageDelay = 1.5f;

Enemy::Enemy()
	: m_state(ENEMY::ALIVE)
	, m_pathingState(ENEMY::SEARCHING)
	, m_fLifeTime(0.f)
{
	m_fSpeed = 10.f;
	m_fAngle = 0.f;
}

Enemy::~Enemy()
{
	if (m_bInitalised)
	{
		delete m_pSprite;
		delete m_pSmokeEmitter;
		m_pSprite = nullptr;
		m_pSmokeEmitter = nullptr;
	}
}

bool Enemy::Initialise(Renderer& renderer, FMOD::System& fmodSystem)
{
	if (!Entity::Initialise(renderer, fmodSystem))
	{
		return false;
	}

	m_pSprite = renderer.CreateSprite("assets\\sprites\\enemy.png", SpriteOrigin::CENTER);
	m_fCollisionRadius = Renderer::ScreenSpaceToWorldSpace(m_pSprite->GetWidth() / 2.f);

	m_collisionState.insert(std::pair<WALL::Direction, bool>(WALL::POSITIVE_HORIZONTAL, false));
	m_collisionState.insert(std::pair<WALL::Direction, bool>(WALL::NEGATIVE_HORIZONTAL, false));
	m_collisionState.insert(std::pair<WALL::Direction, bool>(WALL::POSITIVE_VERTICAL, false));
	m_collisionState.insert(std::pair<WALL::Direction, bool>(WALL::NEGATIVE_VERTICAL, false));

	fmodSystem.createSound("assets\\sounds\\spawn.wav", FMOD_DEFAULT, NULL, &m_pSpawnSound);

	m_pSmokeEmitter = new SmokeEmitter();
	m_pSmokeEmitter->Initialise(renderer, 0.01f, 1.f, 3.f, static_cast<float>(M_PI) * 2.f, 2.f, 0.2f);

	m_bInitalised = true;
	return m_bInitalised;
}

void 
Enemy::Setup(Player& player, std::tuple<unsigned int, Wall*>& geometry, std::tuple<unsigned int, Node*>& nodes)
{
	m_pPlayer = &player;
	m_pGeometry = &geometry;
	m_pPathNodes = &nodes;
}

bool
Enemy::Reinitialise(Vector2 position, float health, Node* closestNodeToPlayer)
{
	m_position = position;
	m_fHealth = health;
	UpdateTravelPath(closestNodeToPlayer);
	m_state = ENEMY::ALIVE;
	m_pathingState = ENEMY::SEARCHING;
	m_fLifeTime = 0.f;

	m_pFMODsystem->playSound(m_pSpawnSound, NULL, false, &m_pSpawnSoundChannel);
	m_pSpawnSoundChannel->setVolume(0.2f);
	m_pSmokeEmitter->SetActive(true);
	m_pSmokeEmitter->SetPosition(position);

	m_bIsLoaded = true;
	return m_bIsLoaded;
}

void Enemy::Process(float deltaTime, Input& input)
{
	m_fLifeTime += deltaTime;

	m_pSmokeEmitter->Process(deltaTime);
	if (m_fLifeTime > beginDamageDelay / 6.f)
	{
		m_pSmokeEmitter->SetActive(false);
	}

	m_pDeathSprite->Process(deltaTime);
	if (m_state == ENEMY::DEAD)
	{
		if (!m_pDeathSprite->IsAnimating())
		{
			Unload();
		}

		return;
	}

	// Collision Detection
	for (unsigned int i = 0; i < std::get<0>(*m_pGeometry); ++i)
	{
		std::tuple<bool, WALL::Direction> collisionState = std::get<1>(*m_pGeometry)[i].IsColliding(m_position, m_fCollisionRadius);
		if (std::get<0>(collisionState))
		{
			SetCollisionState(std::get<1>(collisionState));
		}
	}

	Vector2 playerPosition = m_pPlayer->GetPosition();
	Vector2 currentTarget = playerPosition;
	
	if (m_pathingState == ENEMY::SEARCHING)
	{
		static bool doUpdateTravel = true;
		if (m_travelPath.size() > 0)
		{
			currentTarget = m_travelPath.top();
			if (doUpdateTravel)
			{
				UpdateTravelPath(m_pPlayer->GetClosestNode());
				doUpdateTravel = false;
			}
		}
		else
		{
			doUpdateTravel = true;
		}

		if (m_travelPath.size() > 0 && IsColliding(m_travelPath.top(), 0.2f))
		{
			m_travelPath.pop();
		}

		// Only check for LOS is enemy is close to player
		float enemyToPlayerDistance = (playerPosition - m_position).Length();
		if (enemyToPlayerDistance < 10.f && Wall::IsInLOS(m_position, playerPosition, *m_pGeometry))
		{
			m_pathingState = ENEMY::FOLLOWING;
		}
	}
	else if (m_pathingState == ENEMY::FOLLOWING)
	{
		if (m_velocity.LengthSquared() < Vector2(0.1f, 0.1f).LengthSquared())
		{
			m_pathingState = ENEMY::SEARCHING;
		}
	}


	m_fAngle = atan2f(currentTarget.y - m_position.y, currentTarget.x - m_position.x);
	Vector2 targetVelocity = Vector2(m_fAngle, 3.5f, POLAR);
	m_velocity = Vector2::Lerp(m_velocity, targetVelocity, 5.f * deltaTime);


	if (m_pPlayer->IsColliding(m_position, m_fCollisionRadius) && m_fLifeTime > beginDamageDelay)
	{
		m_pPlayer->Damage(damageAmount);
		m_velocity = { 0.f, 0.f };
	}

	if (m_collisionState[WALL::NEGATIVE_VERTICAL])
	{
		m_velocity.x = Clamp(-m_fSpeed, m_velocity.x, 0);
	}
	if (m_collisionState[WALL::POSITIVE_VERTICAL])
	{
		m_velocity.x = Clamp(0, m_velocity.x, m_fSpeed);
	}
	if (m_collisionState[WALL::NEGATIVE_HORIZONTAL])
	{
		m_velocity.y = Clamp(-m_fSpeed, m_velocity.y, 0);
	}
	if (m_collisionState[WALL::POSITIVE_HORIZONTAL])
	{
		m_velocity.y = Clamp(0, m_velocity.y, m_fSpeed);
	}
	
	if (m_velocity == Vector2(0.f, 0.f))
	{
		UpdateTravelPath(m_pPlayer->GetClosestNode());
	}

	m_position += m_velocity * deltaTime;
}

void 
Enemy::UpdateTravelPath(Node* closestNodeToPlayer)
{
	std::vector<Path> paths;

	SearchForPath(paths, closestNodeToPlayer, NULL, -1, 0,std::get<0>(*m_pPathNodes));

	float closestPath = std::numeric_limits<float>::max();
	std::vector<Path>::iterator path;
	for (path = paths.begin(); path != paths.end(); ++path)
	{
		if (std::get<1>(*path) < closestPath && Wall::IsInLOS(m_position, std::get<0>(*path).top(), *m_pGeometry))
		{
			m_travelPath = std::get<0>(*path);
			closestPath = std::get<1>(*path);
		}
	}
}

void
Enemy::SearchForPath(std::vector<Path>& paths, Node* node, Node* prevNode, int currentPathIndex, unsigned int searchDepth, unsigned int maxSearchDepth)
{
	if (searchDepth >= maxSearchDepth)
	{
		return;
	}

	for (unsigned int index = 0; index < node->GetConnectionCount(); ++index)
	{
		unsigned int pathIndex = currentPathIndex < 0 ? index : currentPathIndex;
		if (paths.size() <= pathIndex)
		{
			Path p = make_tuple(std::stack<Vector2>(), 0.f);
			std::get<0>(p).push(node->GetPosition());
			paths.push_back(p);
		}

		Node* connection = node->GetConnections()[index];
		if (prevNode != NULL && connection->GetId() == prevNode->GetId())
		{
			continue;
		}

		float nodeConnectionDistance = (node->GetPosition() - connection->GetPosition()).LengthSquared();
		float nodeEnemyDistance = (node->GetPosition() - m_position).LengthSquared();
		float enemyConnectionDistance = (connection->GetPosition() - m_position).LengthSquared();

		if (nodeEnemyDistance < nodeConnectionDistance)
		{
			std::get<1>(paths[pathIndex]) += nodeEnemyDistance;
			return;
		}

		std::get<1>(paths[pathIndex]) += nodeConnectionDistance;
		std::get<0>(paths[pathIndex]).push(connection->GetPosition());

		SearchForPath(paths, connection, node, pathIndex, searchDepth + 1, maxSearchDepth);
		std::get<1>(paths[pathIndex]) += nodeEnemyDistance;
	}
}

void 
Enemy::Draw(Renderer& renderer)
{
	float drawAngle = m_velocity == Vector2(0.f, 0.f) ?
		atan2f(m_pPlayer->GetPosition().y - m_position.y, m_pPlayer->GetPosition().x - m_position.x) - static_cast<float>(M_PI) / 2.f :
		atan2f(m_velocity.y, m_velocity.x) - static_cast<float>(M_PI) / 2.f;

	if (m_state != ENEMY::DEAD)
	{
		m_pSprite->Draw(renderer, m_position, drawAngle);
	}
	
	if (m_pDeathSprite->IsAnimating())
	{
		m_pDeathSprite->Draw(renderer, m_position, 0.f);
	}

	m_pSmokeEmitter->Draw(renderer);
}

void
Enemy::Reset()
{
	m_collisionState[WALL::NEGATIVE_VERTICAL] = false;
	m_collisionState[WALL::POSITIVE_VERTICAL] = false;
	m_collisionState[WALL::NEGATIVE_HORIZONTAL] = false;
	m_collisionState[WALL::POSITIVE_HORIZONTAL] = false;
}

#ifdef _DEBUG
void 
Enemy::DebugDraw()
{
	ImGui::Text("Entity Type: Enemy");
	ImGui::Separator();

	ImGui::Text("Movement");
	ImGui::Text("Velocity: {%f, %f}", m_velocity.x, m_velocity.y);
	ImGui::Text("Position: {%f, %f}", m_position.x, m_position.y);
	ImGui::Separator();

}
#endif // _DEBUG

void 
Enemy::Damage(float amount)
{
	m_fHealth -= amount;
	m_pPlayer->Heal(healAmount);
	m_pFMODsystem->playSound(m_pHitSound, NULL, false, &m_pHitSoundChannel);
	m_pHitSoundChannel->setVolume(0.5f);

	if (m_fHealth <= 0.f && m_state != ENEMY::DEAD)
	{
		m_pDeathSprite->Restart();
		m_pDeathSprite->Animate();
		m_state = ENEMY::DEAD;
		m_pFMODsystem->playSound(m_pDeathSound, NULL, false, &m_pDeathSoundChannel);
		m_pDeathSoundChannel->setVolume(0.5f);
		m_pPlayer->IncrementKillCount();
	}
}

void 
Enemy::SetCollisionState(WALL::Direction direction)
{
	m_collisionState[direction] = true;
}

float 
Enemy::GetCollisionRadius()
{
	return Renderer::ScreenSpaceToWorldSpace(m_pSprite->GetWidth() / 2.f);
}