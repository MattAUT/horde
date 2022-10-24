// COMP710 GP Framework 2022
#include "sprite.h"

// Local includes:
#include "renderer.h"
#include "texture.h"

#include <logmanager.h>
#include <utils.h>
#include <vector2.h>
#include <vertexarray.h>

bool Sprite::GenerateVertices(std::vector<float>* vertices, Vector2 topLeftUV, Vector2 topRightUV, Vector2 bottomRightUV, Vector2 bottomLeftUV, SpriteOrigin origin)
{

	float xOffset =
		origin == TOP_LEFT || origin == BOTTOM_LEFT ? 0.5f :
		origin == TOP_RIGHT || origin == BOTTOM_RIGHT ? -0.5f :
		0.f;

	float yOffset =
		origin == TOP_LEFT || origin == TOP_RIGHT || origin == TOP_CENTER ? 1.f : 
		origin == CENTER ? 0.5f :
		0.f;
	

	vertices->assign
	(
		{
			-0.5f + xOffset,  0.0f + yOffset, 0.0f, topLeftUV.x,     topLeftUV.y,     // Top left
			 0.5f + xOffset,  0.0f + yOffset, 0.0f, topRightUV.x,    topRightUV.y,    // Top right
			 0.5f + xOffset, -1.0f + yOffset, 0.0f, bottomRightUV.x, bottomRightUV.y, // Bottom right
			-0.5f + xOffset, -1.0f + yOffset, 0.0f, bottomLeftUV.x,  bottomLeftUV.y   // Bottom left
		}
	);
	
	return true;
}

Sprite::Sprite()
: m_pTexture(0)
, m_width(0)
, m_height(0)
, m_widthScale(1.0f)
, m_heightScale(1.0f)
, m_alpha(1.0f)
, m_tintRed(1.0f)
, m_tintGreen(1.0f)
, m_tintBlue(1.0f)
{

}

Sprite::~Sprite()
{
	delete m_pVertexData;
	m_pVertexData = NULL;
}

bool
Sprite::Initialise(Texture& texture, VertexArray& vertexData)
{
    m_pTexture = &texture;
	m_pVertexData = &vertexData;

    m_width = m_pTexture->GetWidth();
    m_height = m_pTexture->GetHeight();

    return true;
}

void
Sprite::Process(float deltaTime)
{

}

void
Sprite::Draw(Renderer& renderer, Vector2 position, float angle, DrawSpace drawSpace)
{
    m_pTexture->SetActive();
	m_pVertexData->SetActive();
    renderer.DrawSprite(*this, position, angle, drawSpace);
}

void
Sprite::SetScale(float scale)
{
    m_widthScale = scale;
    m_heightScale = scale;
}

void
Sprite::SetScaleX(float scale)
{
	m_widthScale = scale;
}

void
Sprite::SetScaleY(float scale)
{
	m_heightScale = scale;
}

void
Sprite::SetAlpha(float alpha)
{
    m_alpha = Clamp(0.0f, alpha, 1.0f);
}

float
Sprite::GetAlpha() const
{
    return m_alpha;
}

int
Sprite::GetWidth() const
{
    return static_cast<int>(ceilf(m_width * m_widthScale));
}

int
Sprite::GetHeight() const
{
    return static_cast<int>(ceilf(m_height * m_heightScale));
}

void
Sprite::SetRedTint(float value)
{
    m_tintRed = Clamp(0.0f, value, 1.0f);
}

float
Sprite::GetRedTint() const
{
    return m_tintRed;
}

void
Sprite::SetGreenTint(float value)
{
    m_tintGreen = Clamp(0.0f, value, 1.0f);
}

float
Sprite::GetGreenTint() const
{
    return m_tintGreen;
}

void
Sprite::SetBlueTint(float value)
{
    m_tintBlue = Clamp(0.0f, value, 1.0f);
}

float
Sprite::GetBlueTint() const
{
    return m_tintBlue;
}

