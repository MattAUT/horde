#ifndef __SHOTGUN_H__
#define __SHOTGUN_H__

#include "weapon.h"

class Shotgun : public Weapon
{
public:
	Shotgun();
	virtual ~Shotgun();
	virtual bool Initialise(Renderer& renderer, FMOD::System& fmodSystem) override;
	virtual void Process(float deltaTime, Input& input) override;
	virtual void Draw(Renderer& renderer) override;
#ifdef _DEBUG
	virtual void DebugDraw() override;
#endif // _DEBUG
	virtual void Shoot(Vector2 playerPosition, float angle) override;

protected:
private:

public:
protected:
	SpriteSheetAnimation* m_pShotgunFlash;

	Vector2 m_drawPosition;
	float m_fDrawAngle;
private:

};

#endif // __SHOTGUN_H__