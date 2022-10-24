// COMP710 GP Framework 2021
#include "spritesheetanimation.h"

#include "renderer.h"
#include "texture.h"

#include <utils.h>
#include <vertexarray.h>
#include <imgui/imgui.h>
#include <cassert>

SpriteSheetAnimation::SpriteSheetAnimation()
	: m_pVertexData(0)
	, m_iFrameWidth(0)
	, m_iFrameHeight(0)
	, m_iCurrentFrame(0)
	, m_iTotalFrames(0)
	, m_fTimeElapsed(0.0f)
	, m_bAnimating(false)
	, m_bLooping(false)
	, m_frameDuration(1.0f)
	, totalTime(0.0f)
{

}

SpriteSheetAnimation::~SpriteSheetAnimation()
{
	delete m_pVertexData;
	m_pVertexData = 0;
}

bool
SpriteSheetAnimation::Initialise(Texture& texture)
{
	return Sprite::Initialise(texture, *m_pVertexData);
}

int
SpriteSheetAnimation::GetWidth() const
{
	return static_cast<int>(ceilf(m_iFrameWidth * m_widthScale));
}

int
SpriteSheetAnimation::GetHeight() const
{
	return static_cast<int>(ceilf(m_iFrameHeight * m_heightScale));
}

bool
SpriteSheetAnimation::IsAnimating() const
{
	return m_bAnimating;
}

void
SpriteSheetAnimation::SetupFrames(int fixedFrameWidth, int fixedFrameHeight, SpriteOrigin origin)
{
	m_iFrameWidth = fixedFrameWidth;
	m_iFrameHeight = fixedFrameHeight;

	const int textureWidth = m_pTexture->GetWidth();
	const int textureHeight = m_pTexture->GetHeight();

	const int totalFramesWide = textureWidth / fixedFrameWidth;
	const int totalFramesHigh = textureHeight / fixedFrameHeight;

	const int stride = 5;
	const float uFrameWidth = 1.0f / totalFramesWide;
	const float vFrameHeight = 1.0f / totalFramesHigh;

	m_iTotalFrames = totalFramesWide * totalFramesHigh;
	const int vertsPerSprite = 4;
	const int numVertices = vertsPerSprite * (m_iTotalFrames);
	float* vertices = new float[numVertices * stride];

	for (int h = 0; h < totalFramesHigh; ++h)
	{
		for (int w = 0; w < totalFramesWide; ++w)
		{
			float uOffset = (w * uFrameWidth);
			float vOffset = (h * vFrameHeight);

			std::vector<float> quad; 
			Sprite::GenerateVertices
			(
				&quad,
				{ 0.f + uOffset, vOffset + vFrameHeight }, 
				{ uFrameWidth + uOffset, vOffset + vFrameHeight }, 
				{ uFrameWidth + uOffset, vOffset }, 
				{ 0.f + uOffset, vOffset }, 
				origin
			);

			const int floatsPerSprite = stride * vertsPerSprite;
			for (int j = 0; j < floatsPerSprite; ++j)
			{
				int index = ((w * floatsPerSprite) + j) + (h * (floatsPerSprite * totalFramesWide));
				vertices[index] = quad[j];
			}
		}
	}

	const int totalIndices = 6 * m_iTotalFrames;
	unsigned int* allIndices = new unsigned int[totalIndices];
	unsigned int i = 0;

	for (int k = 0; k < m_iTotalFrames; ++k)
	{
		unsigned int indices[] = 
		{ 
			0 + i, 
			1 + i, 
			2 + i, 
			2 + i, 
			3 + i, 
			0 + i
		};

		for (int j = 0; j < 6; ++j)
		{
			int index = (k * 6) + j;
			allIndices[index] = indices[j];
		}

		i += 4;
	}

	m_pVertexData = new VertexArray(vertices, numVertices, allIndices, totalIndices);
	delete vertices;
	vertices = 0;
	delete allIndices;
	allIndices = 0;
}

void
SpriteSheetAnimation::Process(float deltaTime)
{
	totalTime += deltaTime;
	if (m_bAnimating)
	{
		m_fTimeElapsed += deltaTime;
		if (m_fTimeElapsed > m_frameDuration)
		{
			++m_iCurrentFrame;
			if (m_iCurrentFrame >= m_iTotalFrames)
			{
				if (m_bLooping)
				{
					Restart();
				}
				else
				{
					m_iCurrentFrame = m_iTotalFrames - 1;
					m_bAnimating = false;
				}
			}
			m_fTimeElapsed = 0.0f;
		}
	}
}

void
SpriteSheetAnimation::Draw(Renderer& renderer, Vector2 position, float angle, DrawSpace drawSpace)
{
	assert(m_pVertexData);
	m_pTexture->SetActive();
	m_pVertexData->SetActive();
	renderer.DrawAnimatedSprite(*this, position, angle, m_iCurrentFrame, drawSpace);
}

void
SpriteSheetAnimation::Animate()
{
	m_bAnimating = true;
}

void
SpriteSheetAnimation::SetFrameDuration(float seconds)
{
	m_frameDuration = seconds;
}

void
SpriteSheetAnimation::SetLooping(bool loop)
{
	m_bLooping = loop;
}

void
SpriteSheetAnimation::Restart()
{
	m_iCurrentFrame = 0;
	m_fTimeElapsed = 0.0f;
}
void
SpriteSheetAnimation::DebugDraw()
{
	ImGui::SliderInt("Frame ", &m_iCurrentFrame, 0, m_iTotalFrames - 1);
}