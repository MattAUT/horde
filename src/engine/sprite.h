// COMP710 GP Framework 2022
#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "renderer.h" // We need this for enum definitions 

class Texture;
class Vector2;
class VertexArray;

#include <vector>

class Sprite
{
    // Member methods:
public:
    Sprite();
    ~Sprite();

    bool Initialise(Texture& texture, VertexArray& vertexData);
    void Process(float deltaTime);
    void Draw(Renderer& renderer, Vector2 position, float angle, DrawSpace drawSpace = DrawSpace::WORLD_SPACE);

    int GetWidth() const;
    int GetHeight() const;

    void SetScale(float scale);
    void SetScaleX(float scale);
    void SetScaleY(float scale);

    void SetAlpha(float alpha);
    float GetAlpha() const;

    void SetRedTint(float value);
    float GetRedTint() const;
    void SetGreenTint(float value);
    float GetGreenTint() const;
    void SetBlueTint(float value);
    float GetBlueTint() const;

	static bool GenerateVertices
	(
		std::vector<float>* vertices,
		Vector2 topLeftUV, 
		Vector2 topRightUV, 
		Vector2 bottomRightUV, 
		Vector2 bottomLeftUV, 
		SpriteOrigin origin
	);

protected:
private:
    Sprite(const Sprite& sprite);
    Sprite& operator=(const Sprite& sprite) {}

    // Member data:
public:

protected:
    Texture* m_pTexture;
	VertexArray* m_pVertexData;

    int m_width;
    int m_height;

    float m_widthScale;
    float m_heightScale;
    float m_alpha;

    float m_tintRed;
    float m_tintGreen;
    float m_tintBlue;

private:

};

#endif // __SPRITE_H__

