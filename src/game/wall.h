#ifndef __WALL_H__
#define __WALL_H__

class Sprite;
class Renderer;
class Player;

#include <box2d/box2d.h>
#include <vector2.h>
#include <tuple>

namespace WALL 
{
	enum Direction
	{
		POSITIVE_HORIZONTAL,
		POSITIVE_VERTICAL,
		NEGATIVE_HORIZONTAL,
		NEGATIVE_VERTICAL
	};
};

namespace INI 
{
	class Settings;
}

class Wall
{
public:
	Wall();
	~Wall();
	bool Initialise(Renderer& renderer, INI::Settings levelSettings, unsigned int index);
	void Draw(Renderer& renderer);

	std::tuple<bool, WALL::Direction> IsColliding(Vector2 position, float radius);
	static bool IsInLOS(Vector2 a, Vector2 b, std::tuple<unsigned int, Wall*>& geometry);

protected:
private:

public:
protected:
	Sprite* m_pTile;
	float m_fTileRad;

	WALL::Direction m_dir;
	float m_fLength;
	Vector2 m_position;
private:

};

#endif // __WALL_H__