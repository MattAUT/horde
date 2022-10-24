#include "weapon.h"

#include <engine/renderer.h>
#include <engine/spritesheetanimation.h>

float Weapon::sm_fMaxHeat = 100.f;

Weapon::Weapon()
{

}

Weapon::~Weapon()
{
	delete m_pSprite;
	delete m_pIconSprite;
	m_pSprite = nullptr;
	m_pIconSprite = nullptr;
}

void
Weapon::Setup(Enemy* enemies, std::tuple<unsigned int, Wall*>* geometry)
{
	m_pEnemies = enemies;
	m_pGeometry = geometry;
}

void
Weapon::Process(float deltaTime, Input& input)
{
	if (m_fHeat > 0.f)
	{
		m_fHeat -= m_fCooldownSpeed * deltaTime;
	}
	else
	{
		m_fHeat = 0.f;
		m_bOverheated = false;
	}
}

float 
Weapon::GetHeat() const
{
	return m_fHeat;
}

bool
Weapon::IsOverheated() const
{
	return m_bOverheated;
}

Sprite* 
Weapon::GetDrawSprite() const
{
	return m_pSprite;
}

Sprite*
Weapon::GetIconSprite() const
{
	return m_pIconSprite;
}