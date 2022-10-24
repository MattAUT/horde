#include "player.h"

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
#include <limits>
#include <fmod.hpp>

#include "scenelevel.h"
#include "playerui.h"
#include "enemy.h"
#include "smokeemitter.h"

#include "shotgun.h"
#include "rifle.h"
#include "rapid.h"
#include "boom.h"
#include "node.h"

int Player::sm_iWeaponCount = 4;
const float invTimeAmount = 0.5f;
const float walkSoundDelay = 0.25;

Player::Player()
	: m_fInvTime(0.f)
	, m_iActiveWeaponIndex(0)
	, m_uiKillCount(0)
	, m_uiKillCountTotal(0)
	, m_bIsDead(false)
{
	m_fHealth = 100.f;
	m_fSpeed = 10.f;
	m_fAngle = 0.f;
}

Player::~Player()
{
	delete m_pSprite;
	m_pSprite = nullptr;

	for (int i = 0; i < sm_iWeaponCount; ++i)
	{
		delete m_pWeapons[i];
		m_pWeapons[i] = nullptr;
	}

	delete[] m_pWeapons;
	m_pWeapons = nullptr;

	delete m_pPlayerUI;
	m_pPlayerUI = nullptr;

	delete m_pSmokeEmitter;
	m_pSmokeEmitter = nullptr;
}

bool Player::Initialise(Renderer& renderer, FMOD::System& fmodSystem)
{
	if (!Entity::Initialise(renderer, fmodSystem))
	{
		return false;
	}

	m_pPlayerUI = new PlayerUI();
	m_pPlayerUI->Initialise(renderer, *this);

	m_pWeapons = new Weapon*[sm_iWeaponCount];
	m_pWeapons[0] = new Rifle();
	m_pWeapons[1] = new Shotgun();
	m_pWeapons[2] = new Rapid();
	m_pWeapons[3] = new Boom();

	m_pWeapons[0]->Initialise(renderer, fmodSystem);
	m_pWeapons[1]->Initialise(renderer, fmodSystem);
	m_pWeapons[2]->Initialise(renderer, fmodSystem);
	m_pWeapons[3]->Initialise(renderer, fmodSystem);

	m_pSmokeEmitter = new SmokeEmitter();
	m_pSmokeEmitter->Initialise(renderer, 0.01f, 0.25f, 0.5f, static_cast<float>(M_PI), 1.2f, 0.2f);

	m_pSprite = renderer.CreateSprite("assets\\sprites\\player.png", SpriteOrigin::CENTER);
	m_fCollisionRadius = Renderer::ScreenSpaceToWorldSpace(m_pSprite->GetWidth() / 2.f);

	m_collisionState.insert(std::pair<WALL::Direction, bool>(WALL::POSITIVE_HORIZONTAL, false));
	m_collisionState.insert(std::pair<WALL::Direction, bool>(WALL::NEGATIVE_HORIZONTAL, false));
	m_collisionState.insert(std::pair<WALL::Direction, bool>(WALL::POSITIVE_VERTICAL, false));
	m_collisionState.insert(std::pair<WALL::Direction, bool>(WALL::NEGATIVE_VERTICAL, false));

	m_bIsLoaded = true;
	return m_bIsLoaded;
}

void
Player::Setup(Enemy* enemies, std::tuple<unsigned int, Wall*>& geometry, std::tuple<unsigned int, Node*>& nodes)
{
	m_pEnemies = enemies;
	m_pGeometry = &geometry;
	m_pPathNodes = &nodes;
	m_pActiveNode = &std::get<1>(nodes)[0];

	for (int i = 0; i < sm_iWeaponCount; ++i)
	{
		m_pWeapons[i]->Setup(enemies, &geometry);
	}
}

void Player::Process(float deltaTime, Input& input)
{
	m_pDeathSprite->Process(deltaTime);
	m_pSmokeEmitter->Process(deltaTime);
	m_pSmokeEmitter->SetPosition(m_position);
	m_pSmokeEmitter->SetAngle(atan2(m_velocity.y, m_velocity.x) - static_cast<float>(M_PI));

	static XInputController* controller = input.GetController(0);
	static bool hasSwitched = false;

	for (int i = 0; i < sm_iWeaponCount; ++i)
	{
		m_pWeapons[i]->Process(deltaTime, input);
		if (dynamic_cast<Rapid*>(m_pWeapons[i]) != nullptr)
		{
			dynamic_cast<Rapid*>(m_pWeapons[i])->SetAutoAttributes(m_position, m_fAngle);
		}
	}

	m_pPlayerUI->Process(deltaTime);

	if (m_fInvTime > 0.f)
	{
		m_fInvTime -= deltaTime;
	}

	if (m_bIsDead)
	{
		return;
	}

	// Inputs
	if (input.GetNumberOfControllersAttached() > 0)
	{
		m_movementInput = controller->GetLeftStick() / std::numeric_limits<int16_t>::max();
		m_rotationInput = controller->GetRightStick() / std::numeric_limits<int16_t>::max();

		if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == BS_PRESSED)
		{
			Shoot();
		}

		float rightTrigger = controller->GetRightTrigger() / std::numeric_limits<int16_t>::max();
		float leftTrigger = controller->GetLeftTrigger() / std::numeric_limits<int16_t>::max();

		int switchDirection = rightTrigger > leftTrigger ? 1 : -1;

		if (rightTrigger || leftTrigger)
		{
			if (!hasSwitched)
			{
				m_iActiveWeaponIndex += switchDirection;

				if (m_iActiveWeaponIndex >= sm_iWeaponCount)
				{
					m_iActiveWeaponIndex = 0;
				}

				if (m_iActiveWeaponIndex < 0)
				{
					m_iActiveWeaponIndex = sm_iWeaponCount - 1;
				}

				hasSwitched = true;
			}
		}
		else {
			hasSwitched = false;
		}

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
	
	bool setToConnection = false;
	ID lastActiveNodeId = m_pActiveNode->GetId();
	float currentDist = (m_pActiveNode->GetPosition() - m_position).LengthSquared();

	for (unsigned int i = 0; i < m_pActiveNode->GetConnectionCount(); ++i)
	{
		Vector2 nodePos = (m_pActiveNode->GetConnections()[i])->GetPosition();
		float dist = (nodePos - m_position).LengthSquared();
		float currentDist = (m_pActiveNode->GetPosition() - m_position).LengthSquared();
	
		if (dist < currentDist && Wall::IsInLOS(m_position, nodePos, *m_pGeometry))
		{
			m_pActiveNode = m_pActiveNode->GetConnections()[i];		
			setToConnection = true;
		}
	}

	// We only want to search through all the nodes in the
	// rare event a connection node can't be found
	if (!setToConnection)
	{
		for (unsigned int i = 0; i < std::get<0>(*m_pPathNodes); ++i)
		{
			Vector2 nodePos = std::get<1>(*m_pPathNodes)[i].GetPosition();
			float dist = (nodePos - m_position).LengthSquared();
			
			if (dist < currentDist)
			{
				m_pActiveNode = &std::get<1>(*m_pPathNodes)[i];
			}
		}
	}

	if (!(lastActiveNodeId == m_pActiveNode->GetId()))
	{
		for (unsigned int enemyIndex = 0; enemyIndex < SceneLevel::sm_enemyLoadCount; ++enemyIndex)
		{
			if (m_pEnemies[enemyIndex].IsLoaded())
			{
				m_pEnemies[enemyIndex].UpdateTravelPath(m_pActiveNode);
			}
		}
	}

	static float timeUntilWalkSound = 0.f;
	// Movement
	if (m_movementInput.LengthSquared() > powf(0.4f, 2))
	{
		// Effects
		timeUntilWalkSound -= deltaTime;
		if (timeUntilWalkSound <= 0.f)
		{
			m_pSmokeEmitter->SetActive(true);
			m_pFMODsystem->playSound(m_pWalkSound, NULL, false, &m_pWalkSoundChannel);
			m_pWalkSoundChannel->setVolume(0.3f);
			timeUntilWalkSound = walkSoundDelay;
		}

		m_velocity = { m_movementInput.x * m_fSpeed, -m_movementInput.y * m_fSpeed };

	}
	else
	{
		m_velocity = { 0, 0 };
		m_pSmokeEmitter->SetActive(false);
	}

	if (m_rotationInput.LengthSquared() > powf(0.8f, 2))
	{
		float angleToLerpTo = -atan2f(m_rotationInput.y, m_rotationInput.x) - static_cast<float>(M_PI) / 2.f;
		if (abs(m_fAngle - angleToLerpTo) > static_cast<float>(M_PI) * 1.5f && deltaTime > 0.f)
		{
			m_fAngle = angleToLerpTo;
		}
		else 
		{
			m_fAngle = Lerp(m_fAngle, angleToLerpTo, 50.f * deltaTime);
		}
		
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
	
	m_position += m_velocity * deltaTime;
}

void 
Player::Draw(Renderer& renderer)
{
	m_pSmokeEmitter->Draw(renderer);

	Vector2 weaponPosition = m_position + Vector2(m_fAngle + static_cast<float>(M_PI) / 2.f, 0.3f, POLAR); 
	if (!m_bIsDead)
	{
		m_pSprite->Draw(renderer, m_position, m_fAngle);
		m_pWeapons[m_iActiveWeaponIndex]->GetDrawSprite()->Draw(renderer, weaponPosition, m_fAngle);
	}

	if (m_pDeathSprite->IsAnimating())
	{
		m_pDeathSprite->Draw(renderer, m_position, 0.f);
	}

	for (int i = 0; i < sm_iWeaponCount; ++i)
	{
		m_pWeapons[i]->Draw(renderer);
	}

	m_pPlayerUI->Draw(renderer);
}

void
Player::Reset()
{
	m_collisionState[WALL::NEGATIVE_VERTICAL] = false;
	m_collisionState[WALL::POSITIVE_VERTICAL] = false;
	m_collisionState[WALL::NEGATIVE_HORIZONTAL] = false;
	m_collisionState[WALL::POSITIVE_HORIZONTAL] = false;
}

#ifdef _DEBUG
void 
Player::DebugDraw()
{
	ImGui::Text("Player");
	ImGui::Separator();

	ImGui::Text("Movement");
	ImGui::Spacing();
	ImGui::Text("Input Vector: {%f, %f}", m_movementInput.x, m_movementInput.y);
	ImGui::Text("Velocity: {%f, %f}", m_velocity.x, m_velocity.y);
	ImGui::Text("Position: {%f, %f}", m_position.x, m_position.y);
	ImGui::Text("Angle: {%f}", m_fAngle);
	ImGui::Separator();

	ImGui::Text("Health");
	ImGui::Spacing();
	ImGui::Text("Health: %f", m_fHealth);
	ImGui::Text("Invincibility Time: %f", m_fInvTime);
	ImGui::Separator();

	ImGui::Text("Active Weapon");
	m_pWeapons[m_iActiveWeaponIndex]->DebugDraw();
	ImGui::Text("Kill Count: %d", m_uiKillCount);
	ImGui::Separator();

	ImGui::Text("Collision");
	ImGui::Spacing();
	ImGui::Text("Negative Vertical: %s", m_collisionState[WALL::NEGATIVE_VERTICAL] ? "true" : "false");
	ImGui::Text("Positive Vertical: %s", m_collisionState[WALL::POSITIVE_VERTICAL] ? "true" : "false");
	ImGui::Text("Negative Horizontal: %s", m_collisionState[WALL::NEGATIVE_HORIZONTAL] ? "true" : "false");
	ImGui::Text("Positive Horizontal: %s", m_collisionState[WALL::POSITIVE_HORIZONTAL] ? "true" : "false");
	ImGui::Separator();

	ImGui::Text("Nodes");
	ImGui::Spacing();
	ImGui::Text("Active Node Position: {%f, %f}", m_pActiveNode->GetPosition().x, m_pActiveNode->GetPosition().y);
}
#endif // _DEBUG

void
Player::Shoot()
{
	m_pWeapons[m_iActiveWeaponIndex]->Shoot(m_position, m_fAngle);
}

void 
Player::Damage(float amount, bool bypass)
{
	if (m_fInvTime <= 0.f && !m_bIsDead || bypass)
	{
		m_fHealth -= amount;
		m_fInvTime = invTimeAmount;

		if (!bypass)
		{
			m_pPlayerUI->DoDamageFlash();
			m_pFMODsystem->playSound(m_pHitSound, NULL, false, &m_pHitSoundChannel);
			m_pHitSoundChannel->setVolume(0.5f);

		}
	
		if (m_fHealth <= 0.f)
		{
			m_pFMODsystem->playSound(m_pDeathSound, NULL, false, &m_pDeathSoundChannel);
			m_pDeathSoundChannel->setVolume(0.5f);
			m_pDeathSprite->Restart();
			m_pDeathSprite->Animate();
			m_bIsDead = true;
		}
	}
	
}

void
Player::Heal(float amount)
{
	if (m_fHealth >= 100.f)
	{
		m_fHealth = 100.f;
		return;
	}

	m_fHealth += amount;
}

float
Player::GetHealth() const
{
	return m_fHealth;
}

void 
Player::SetCollisionState(WALL::Direction direction)
{
	m_collisionState[direction] = true;
}

Weapon& 
Player::GetActiveWeapon() const
{
	return *m_pWeapons[m_iActiveWeaponIndex];
}


Node*
Player::GetClosestNode()
{
	return m_pActiveNode;
}

void 
Player::IncrementKillCount()
{
	++m_uiKillCount;
	++m_uiKillCountTotal;
}
void 
Player::ResetKillCount(unsigned int count)
{
	m_uiKillCount = count;
}

unsigned int 
Player::GetKillCount() const
{
	return m_uiKillCount;
}

unsigned int
Player::GetKillCountTotal() const
{
	return m_uiKillCountTotal;
}

bool
Player::IsDead() const
{
	return m_bIsDead;
}