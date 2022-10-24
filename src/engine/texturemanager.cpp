// COMP710 GP Framework 2022
#include "texturemanager.h"

#include "texture.h"

#include <logmanager.h>
#include <cassert>
#include <SDL.h>
#ifdef _DEBUG
#include <imgui/imgui_impl_sdl.h>
#endif // _DEBUG

TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()
{
    std::map<std::string, Texture*>::iterator iter = m_pLoadedTextures.begin();

    while (iter != m_pLoadedTextures.end())
    {
        Texture* pTexture = iter->second;
        delete pTexture;

        ++iter;
    }

    m_pLoadedTextures.clear();
}

bool 
TextureManager::Initialise()
{
    LogManager::GetInstance().Log("TextureManager Initalised");
    return true;
}

void
TextureManager::AddTexture(const char* key, Texture* pTexture)
{
	m_pLoadedTextures[key] = pTexture;
}

void
TextureManager::RemoveTexture(const char* key)
{
	delete m_pLoadedTextures[key];
	m_pLoadedTextures.erase(key);
}

Texture*
TextureManager::GetTexture(const char* pcFilename)
{
    Texture* pTexture = 0;

    if (m_pLoadedTextures.find(pcFilename) == m_pLoadedTextures.end())
    {
        // Not already loaded... so load...
        pTexture = new Texture();
        if (!pTexture->LoadImageTexture(pcFilename))
        {
            LogManager::GetInstance().Log("Texture failed to initialise!");
            assert(0);
        }

        m_pLoadedTextures[pcFilename] = pTexture;
    }
    else
    {
        // It has already been loaded...
        pTexture = m_pLoadedTextures[pcFilename];
    }

    return pTexture;
}

Texture*
TextureManager::GetTextTexture(const char* text, TTF_Font* font, SDL_Color color)
{
	Texture* pTexture = 0;

	if (m_pLoadedTextures.find(text) == m_pLoadedTextures.end())
	{
		// Not already loaded... so load...
		pTexture = new Texture();
		pTexture->LoadTextTexture(text, font, color);
		m_pLoadedTextures[text] = pTexture;
	}
	else
	{
		// It has already been loaded...
		pTexture = m_pLoadedTextures[text];
	}

	return pTexture;
}

#ifdef _DEBUG
void
TextureManager::DebugDraw()
{
	ImGui::Begin("TextureManager");
	ImGui::Text("Storing %d Textures", m_pLoadedTextures.size());

	std::map<std::string, Texture*>::iterator iter = m_pLoadedTextures.begin();

	while (iter != m_pLoadedTextures.end())
	{
		Texture* pTexture = iter->second;
		pTexture->DebugDraw();

		++iter;
	}
	ImGui::End();
}
#endif // _DEBUG
