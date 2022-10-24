#ifndef __RAPID_H__
#define __RAPID_H__

#include "weapon.h"

class Rapid : public Weapon
{
public:
	Rapid();
	virtual ~Rapid();
	virtual bool Initialise(Renderer& renderer, FMOD::System& fmodSystem) override;
	virtual void Process(float deltaTime, Input& input) override;
	virtual void Draw(Renderer& renderer) override;
#ifdef _DEBUG
	virtual void DebugDraw() override;
#endif // _DEBUG
	virtual void Shoot(Vector2 playerPosition, float angle) override;
	void SetAutoAttributes(Vector2 position, float angle);

protected:
private:

public:
protected:
	static const unsigned int sm_uiLoadedShots = 32;

	Sprite* m_pShotSprite;
	Shot m_shots[sm_uiLoadedShots];
	
	Vector2 m_autoShotPosition;
	float m_fAutoShotAngle;

	bool m_bShooting;

private:

};

#endif // __RAPID_H__