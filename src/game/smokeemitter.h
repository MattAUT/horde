#ifndef __SMOKEEMITTER_H__ 
#define __SMOKEEMITTER_H__ 

// Library includes: 
#include <vector> 
#include <vector2.h>

// Forward declarations:  
class Renderer;
class Sprite;
class Particle;

// Class declaration:  
class SmokeEmitter
{
	// Member methods:  
public:
	SmokeEmitter();
	~SmokeEmitter();

	bool Initialise(Renderer& renderer, float emitRate, float decceleration, float initalVelocity, float spread, float scale, float lifeSpan);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);

	void Spawn();
#ifdef _DEBUG
	void DebugDraw();
#endif // _DEBUG

	void SetPosition(const Vector2& position);
	void SetAngle(float angle);
	void SetActive(bool active);

protected:

private:
	SmokeEmitter(const SmokeEmitter& e);
	SmokeEmitter& operator=(const SmokeEmitter& e) {}

	// Member data:  
public:

protected:
	static const unsigned int sm_uiTotalParticles = 100;
	Sprite* m_pSharedSprite;
	Particle* m_particles;
	Vector2 m_position;

	float m_fTimeUntilNextSpawn;
	bool m_bIsActive;

	int m_iSpawnBatchSize;
	float m_fEmitRate;
	float m_fMaxLifespan;
	float m_fDeccelerationScalar;
	float m_fInitialVelocityScalar;
	float m_fAngle;
	float m_fSpread;
	float m_fScale;

private:

};

#endif // __SMOKEEMITTER_H__ 