#ifndef __CRATE_H__
#define __CRATE_H__

class Sprite;

#include <engine/entity.h>

class Crate : public Entity
{
	// Methods
public:
	Crate();
	virtual ~Crate();
	virtual bool Initialise(Renderer& renderer, b2World& physWorld, FMOD::System& fmodSystem) override;
	void Reset(Vector2 position);
	virtual void Process(float deltaTime, Input& input) override;
	virtual void Draw(Renderer& renderer) override;
#ifdef _DEBUG
	virtual void DebugDraw() override;
#endif // _DEBUG

	virtual Vector2 GetPosition() override;

protected:

private:
	Crate(const Crate& c);
	Crate& operator=(const Crate& c) {}

	// Data:
public:
protected:
	b2World* m_pb2World;
	b2Body* m_pPhysBody;
	Sprite* m_pCrateSprite;

	bool m_bRunPhysics;

private:
};

#endif // __CRATE_H__