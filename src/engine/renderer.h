// COMP710 GP Framework 2022
#ifndef __RENDERER_H__
#define __RENDERER_H__

// Forward Declarations:
class TextureManager;
class Shader;
class Camera;
class VertexArray;
class Sprite;
class SpriteSheetAnimation;
struct SDL_Window;

#include <vector2.h>
#include <SDL.h>
#include <SDL_ttf.h>

enum SpriteOrigin
{
	BOTTOM_LEFT,
	BOTTOM_RIGHT,
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_CENTER,
	TOP_CENTER,
	CENTER
};

enum DrawSpace
{
	SCREEN_SPACE,
	WORLD_SPACE
};

enum FontSize
{
	SMALL,
	MEDIUM,
	LARGE
};

class Renderer
{
    // Member methods:
public:
    Renderer();
    ~Renderer();

    bool Initialise(bool windowed, Camera* camera, int width = 0, int height = 0);

    void Clear();
    void Present();

    void SetClearColour(unsigned char r, unsigned char g, unsigned char b);
    void GetClearColour(unsigned char& r, unsigned char& g, unsigned char& b);

    int GetWidth() const;
    int GetHeight() const;

    Sprite* CreateSprite(const char* pcFilename, SpriteOrigin origin);
	Sprite* CreateStaticTextSprite(const char* text, FontSize fSize, SDL_Color color, SpriteOrigin origin);
	SpriteSheetAnimation* CreateAnimatedSprite(const char* pcFilename);
    void DrawSprite(Sprite& sprite, Vector2 position, float angle, DrawSpace drawSpace = DrawSpace::WORLD_SPACE);
	void DrawAnimatedSprite(SpriteSheetAnimation& sprite, Vector2& position, float angle, int frame, DrawSpace drawSpace = DrawSpace::WORLD_SPACE);
	void DrawDynamicText(const char* text, FontSize fSize, Vector2& position, SDL_Color color, SpriteOrigin origin);
#ifdef _DEBUG
	void DebugDraw();
#endif // _DEBUG

	static float ScreenSpaceToWorldSpace(float point);
	static float WorldSpaceToScreenSpace(float point);

protected:
    bool InitialiseOpenGL(int screenWidth, int screenHeight);
    void SetFullscreen(bool fullscreen); 
    void LogSdlError();
    bool SetupSpriteShader();
	TTF_Font* GetFontFromEnum(FontSize fSize);

private:
    Renderer(const Renderer& renderer);
	Renderer& operator=(const Renderer& renderer) {}

    // Member data:
public:

protected: 
    TextureManager* m_pTextureManager;
    SDL_Window* m_pWindow;
    SDL_GLContext m_glContext;
	Camera* m_pCamera;

	TTF_Font* m_pFontLarge;
	TTF_Font* m_pFontMedium;
	TTF_Font* m_pFontSmall;

    Shader* m_pSpriteShader;
    VertexArray* m_pSpriteVertexData;

    int m_iWidth;
    int m_iHeight;

    float m_fClearRed;
    float m_fClearGreen;
    float m_fClearBlue;

	static const float sm_WorldSpaceFactor;

private:

};

#endif // __RENDERER_H__
