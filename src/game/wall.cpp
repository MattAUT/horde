#include "wall.h"

#include <engine/renderer.h>
#include <engine/sprite.h>
#include <engine/scene.h>
#include <ini.h>

#include "player.h"

Wall::Wall()
{

}

Wall::~Wall()
{
	delete m_pTile;
	m_pTile = nullptr;
}

bool 
Wall::Initialise(Renderer& renderer, INI::Settings levelSettings, unsigned int index)
{
	char wallXpos[16];
	char wallYpos[16];
	char wallLength[16];
	char wallDir[16];

	sprintf_s(wallXpos, "%dx", index);
	sprintf_s(wallYpos, "%dy", index);
	sprintf_s(wallLength, "%dlen", index);
	sprintf_s(wallDir, "%ddir", index);

	m_position =
	{
		levelSettings.GetFloat("geometry", wallXpos),
		levelSettings.GetFloat("geometry", wallYpos)
	};

	std::string wallDirAsString = levelSettings.GetString("geometry", wallDir);
	m_dir =
		wallDirAsString == "x+" ? WALL::POSITIVE_HORIZONTAL :
		wallDirAsString == "x-" ? WALL::NEGATIVE_HORIZONTAL :
		wallDirAsString == "y+" ? WALL::POSITIVE_VERTICAL :
		WALL::NEGATIVE_VERTICAL;

	m_fLength = levelSettings.GetFloat("geometry", wallLength);

	m_pTile = renderer.CreateSprite("assets\\sprites\\wall.png", SpriteOrigin::CENTER);
	m_fTileRad = Renderer::ScreenSpaceToWorldSpace(m_pTile->GetHeight() / 2.f);

	return true;
}

void 
Wall::Draw(Renderer& renderer)
{
	for (float i = 0; i < m_fLength; ++i)
	{

		bool x = (m_dir == WALL::NEGATIVE_HORIZONTAL || m_dir == WALL::POSITIVE_HORIZONTAL);
		bool y = (m_dir == WALL::NEGATIVE_VERTICAL || m_dir == WALL::POSITIVE_VERTICAL);
		Vector2 pos = 
		{ 
			m_position.x + (x ? i : 0.f),
			m_position.y + (y ? i : 0.f),
		}; 
		m_pTile->Draw(renderer, pos, 0.f);
	}
	
}

std::tuple<bool, WALL::Direction>
Wall::IsColliding(Vector2 position, float radius)
{

	if 
	(
		m_dir == WALL::POSITIVE_HORIZONTAL && 
		position.y - radius < m_position.y + m_fTileRad &&
		position.y > m_position.y - m_fTileRad &&
		position.x + radius > m_position.x - m_fTileRad &&
		position.x - radius < m_position.x + (m_fTileRad * 2.f * m_fLength - m_fTileRad)
	)
	{
		return { true, m_dir };
	}

	if
	(
		m_dir == WALL::NEGATIVE_HORIZONTAL &&
		position.y < m_position.y &&
		position.y + radius > m_position.y - m_fTileRad &&
		position.x + radius > m_position.x - m_fTileRad &&
		position.x - radius < m_position.x + (m_fTileRad * 2.f * m_fLength - m_fTileRad)
	)
	{
		return { true, m_dir };
	}

	if
	(
		m_dir == WALL::POSITIVE_VERTICAL &&
		position.x > m_position.x &&
		position.x - radius < m_position.x + m_fTileRad &&
		position.y + radius > m_position.y - m_fTileRad &&
		position.y - radius < m_position.y + (m_fTileRad * 2.f * m_fLength - m_fTileRad)
	)
	{
		return { true, m_dir };
	}

	if
	(
		m_dir == WALL::NEGATIVE_VERTICAL &&
		position.x < m_position.x &&
		position.x + radius > m_position.x - m_fTileRad &&
		position.y + radius > m_position.y - m_fTileRad &&
		position.y - radius < m_position.y + (m_fTileRad * 2.f * m_fLength - m_fTileRad)
	)
	{
		return { true, m_dir };
	}

	return { false, m_dir };
}

const float LOSprecision = 2.f;

bool
Wall::IsInLOS(Vector2 a, Vector2 b, std::tuple<unsigned int, Wall*>& geometry)
{
	float diffX = a.x - b.x;
	float diffY = a.y - b.y;
	int iterations = static_cast<int>((a - b).Length() * LOSprecision) + 1;

	for (int iter = 0; iter < iterations; ++iter)
	{
		Vector2 posCheck = { b.x + (diffX / iterations) * iter, b.y + (diffY / iterations) * iter };

		for (unsigned int i = 0; i < std::get<0>(geometry); ++i)
		{
			std::tuple<bool, WALL::Direction> collisionState = std::get<1>(geometry)[i].IsColliding(posCheck, 0.5f);
			if (std::get<0>(collisionState))
			{
				return false;
			}
		}
	}

	return true;
}
