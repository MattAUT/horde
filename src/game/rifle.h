#ifndef __RIFLE_H__
#define __RIFLE_H__

#include "weapon.h"

class Rifle : public Weapon
{
public:
	Rifle();
	virtual ~Rifle();
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
	static const unsigned int sm_uiLoadedShots = 8;

	Sprite* m_pShotSprite;
	Shot m_shots[sm_uiLoadedShots];

private:

};

#endif // __RIFLE_H__