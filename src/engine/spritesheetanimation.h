// COMP710 GP Framework 2022
#ifndef __ANIMATEDSPRITE_H__
#define __ANIMATEDSPRITE_H__

// Local includes:
#include "sprite.h"

// Forward declarations: 
class Renderer;
class VertexArray;
// Class declaration: 
class SpriteSheetAnimation : public Sprite
{
	// Member methods: 
public:
	SpriteSheetAnimation();
	~SpriteSheetAnimation();

	bool Initialise(Texture& texture);
	void SetupFrames(int fixedFrameWidth, int fixedFrameHeight, SpriteOrigin origin);
	void Process(float deltaTime);
	void Draw(Renderer& renderer, Vector2 position, float angle, DrawSpace drawSpace = DrawSpace::WORLD_SPACE);
	void DebugDraw();

	void SetLooping(bool loop);
	void Animate();
	bool IsAnimating() const;
	void Restart();
	void SetFrameDuration(float seconds);

	int GetWidth() const;
	int GetHeight() const;

protected:
private:
	SpriteSheetAnimation(const SpriteSheetAnimation& s);
	SpriteSheetAnimation& operator=(const SpriteSheetAnimation& s) {}

	// Member data: 
public:
protected:
	VertexArray* m_pVertexData;
	int m_iFrameWidth;
	int m_iFrameHeight;
	int m_iCurrentFrame;
	int m_iTotalFrames;
	float m_fTimeElapsed;
	float m_frameDuration;
	float totalTime;
	bool m_bAnimating;
	bool m_bLooping;
private:
};
#endif // __ANIMATEDSPRITE_H__