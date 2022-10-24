// COMP710 GP Framework 2022
#include "texture.h"

#include <logmanager.h>
#include <SDL_image.h>
#include <cassert>
#include "GL\glew.h"
#ifdef _DEBUG
#include <imgui/imgui_impl_sdl.h>
#endif // _DEBUG

Texture::Texture()
: m_uiTextureId(0) 
, m_iHeight(0)
, m_iWidth(0)
{

}

Texture::~Texture()
{
    glDeleteTextures(1, &m_uiTextureId);
}

bool 
Texture::Initialise(SDL_Surface* pSurface)
{
    if (pSurface)
    {
        m_iWidth = pSurface->w;
        m_iHeight = pSurface->h;

        int bytesPerPixel = pSurface->format->BytesPerPixel;

        unsigned int format = 0;

        if (bytesPerPixel == 3)
        {
            format = GL_RGB;
        }
        else if (bytesPerPixel == 4)
        {
            format = GL_RGBA;
        }

        glGenTextures(1, &m_uiTextureId);
        glBindTexture(GL_TEXTURE_2D, m_uiTextureId);

        glTexImage2D(GL_TEXTURE_2D, 0, format, m_iWidth, m_iHeight, 0, format, GL_UNSIGNED_BYTE, pSurface->pixels);

        SDL_FreeSurface(pSurface);
        pSurface = 0;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    else
    {
        LogManager::GetInstance().Log("Texture failed to load!");
        assert(0);
        return false;
    }

    return true;
}

bool
Texture::LoadImageTexture(const char* pcFilename)
{
	SDL_Surface* pSurface = IMG_Load(pcFilename);
	return Initialise(pSurface);
}

bool
Texture::LoadTextTexture(const char* text, TTF_Font* pFont, SDL_Color color)
{
	SDL_Surface* pSurface = TTF_RenderText_Blended(pFont, text, color);
	return Initialise(pSurface);
}

void
Texture::SetActive()
{
    glBindTexture(GL_TEXTURE_2D, m_uiTextureId);
}

int 
Texture::GetWidth() const
{
    assert(m_iWidth); 
    return (m_iWidth);
}

int 
Texture::GetHeight() const
{
    assert(m_iHeight);
    return (m_iHeight);
}


#ifdef _DEBUG
void 
Texture::DebugDraw()
{
	ImGui::PushID(this);
	ImGui::Text("Size: (%dx%d)", m_iWidth, m_iHeight);
	ImVec2 size;
	float aspectRatio = static_cast<float>(m_iWidth) / static_cast<float>(m_iHeight);
	size.x = sm_iDebugTextureSize * aspectRatio;
	size.y = sm_iDebugTextureSize;
	ImGui::Image((void*)(intptr_t)m_uiTextureId, size);
	ImGui::PopID();
}
#endif // _DEBUG
