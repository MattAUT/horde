// COMP710 GP Framework 2022

#include "round.h"
#include <utils.h>
#include <cmath>

#ifdef _DEBUG
#include <imgui/imgui_impl_sdl.h>
#endif // _DEBUG


Round::Round()
 	: m_uiRoundCount(1)
{
	SetParameters();
}

Round::~Round()
{

}

void
Round::Increment()
{
	++m_uiRoundCount;
	SetParameters();
}

unsigned int
Round::GetCurrentRound() const
{
	return m_uiRoundCount;
}

float
Round::GetSpawnDelay() const
{
	return m_fSpawnDelay;
}

float 
Round::GetSpawnHealth() const
{
	return m_fSpawnHealth;
}

unsigned int 
Round::GetSpawnCount() const
{
	return m_uiSpawnCount;
}
void
Round::SetParameters()
{
	m_uiSpawnCount = static_cast<unsigned int>(
		0.000006f * powf(static_cast<float>(m_uiRoundCount), 3.f) + 
		0.07f * powf(static_cast<float>(m_uiRoundCount), 2.f) +
		0.72f * static_cast<float>(m_uiRoundCount) + 8.f);
	m_fSpawnHealth = powf(m_uiRoundCount / 1.3f, 2) + 50.f;
	m_fSpawnDelay = Clamp(0.5f, -sqrtf(static_cast<float>(m_uiRoundCount) / 10.f) + 2.f, 2.f);
}