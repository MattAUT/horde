#ifndef __BOOM_H__
#define __BOOM_H__

#include "weapon.h"

class Boom : public Weapon
{
public:
	Boom();
	virtual ~Boom();
	virtual bool Initialise(Renderer& renderer, FMOD::System& fmodSystem) override;
	virtual void Process(float deltaTime, Input& input) override;
	virtual void Draw(Renderer& renderer) override;
#ifdef _DEBUG
	virtual void DebugDraw() override;
#endif // _DEBUG
	virtual void Shoot(Vector2 playerPosition, float angle) override;
	void Explode(Vector2 position);

protected:
private:

public:
protected:
	static const unsigned int sm_uiLoadedShots = 2;

	SpriteSheetAnimation* m_pShotSprite;
	SpriteSheetAnimation* m_pExplosionSprite;

	Shot m_shots[sm_uiLoadedShots];

	Vector2 m_explosionDrawPosition;
	FMOD::Sound* m_BoomSound;
	FMOD::Channel* m_BoomSoundChannel;
private:

};

#endif // __BOOM_H__