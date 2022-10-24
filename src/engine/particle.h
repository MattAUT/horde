#ifndef __PARTICLE_H__ 
#define __PARTICLE_H__ 

// Local includes: 
#include <vector2.h>

// Forward declarations:  
class Renderer;
class Sprite;

// Class declaration:  
class Particle
{
	// Member methods:  
public:
	Particle();
	~Particle();

	bool Initialise(Sprite& sprite);
	void Setup(Vector2 position, Vector2 velocity, Vector2 acceleration, float lifeSpan, float minScale, float maxScale);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	bool IsAlive() const;

protected:

private:
	Particle(const Particle& particle);
	Particle& operator=(const Particle& particle);

	// Member data:  
public:
	Sprite* m_pSharedSprite;
	Vector2 m_postion;
	Vector2 m_velocity;
	Vector2 m_acceleration;
	float m_fMaxLifespan;
	float m_fCurrentAge;
	float m_fAngle;
	float m_fScale;
	bool m_bAlive;

protected:

private:

};

#endif // __PARTICLE_H__ 