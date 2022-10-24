// COMP710 GP Framework 2022
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

struct SDL_Surface;

#include <SDL_ttf.h>

class Texture
{
    // Member methods:
public:
    Texture();
    ~Texture();

    bool LoadImageTexture(const char* pcFilename);
	bool LoadTextTexture(const char* text, TTF_Font* pFont, SDL_Color color);

    void SetActive();

    int GetWidth() const;
    int GetHeight() const;

#ifdef _DEBUG
	void DebugDraw();
#endif // _DEBUG

protected:
	bool Initialise(SDL_Surface* pSurface);
private:
    Texture(const Texture& texture);
    Texture& operator=(const Texture& texture);

    // Member data:
public:

protected:
    unsigned int m_uiTextureId;
    int m_iWidth;
    int m_iHeight;

#ifdef _DEBUG
	static const int sm_iDebugTextureSize = 50;
#endif // _DEBUG
private:

};

#endif // __TEXTURE_H__
