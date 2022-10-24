// COMP710 GP Framework 2022

// This include:
#include "renderer.h"

// Local includes:
#include "texture.h"
#include "texturemanager.h"
#include "shader.h"
#include "sprite.h"
#include "scene.h"
#include "camera.h"
#include "spritesheetanimation.h"

// Library includes:
#include <logmanager.h>
#include <vertexarray.h>
#include <matrix4.h>
#include <utils.h>
#include <SDL.h>
#include <SDL_image.h>
#include <GL\glew.h>
#include <cassert>
#include <cmath>
#ifdef _DEBUG
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>
#endif // _DEBUG

const float Renderer::sm_WorldSpaceFactor = 32;

Renderer::Renderer()
: m_pTextureManager(0)
, m_pSpriteShader(0)
, m_pSpriteVertexData(0)
, m_glContext(0)
, m_iWidth(0)
, m_iHeight(0)
, m_fClearRed(0.0f)
, m_fClearGreen(0.0f)
, m_fClearBlue(0.0f)
{

}

Renderer::~Renderer()
{
#ifdef _DEBUG
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
#endif // _DEBUG

    delete m_pSpriteShader;
    m_pSpriteShader = 0;

    delete m_pSpriteVertexData;
    m_pSpriteVertexData = 0;

    delete m_pTextureManager;
    m_pTextureManager = 0;

    SDL_DestroyWindow(m_pWindow);
    IMG_Quit();
	TTF_CloseFont(m_pFontSmall);
	TTF_CloseFont(m_pFontMedium);
	TTF_CloseFont(m_pFontLarge);
	m_pFontSmall = NULL;
	m_pFontMedium = NULL;
	m_pFontLarge = NULL;
	TTF_Quit();
    SDL_Quit();
}

float
Renderer::ScreenSpaceToWorldSpace(float point)
{
	return point / sm_WorldSpaceFactor;
}


float
Renderer::WorldSpaceToScreenSpace(float point)
{
	return point * sm_WorldSpaceFactor;
}


bool 
Renderer::Initialise(bool windowed, Camera* camera, int width, int height)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        LogSdlError();
        return false;
    }

    if (!windowed)
    {
        // Go fullscreen, with current resolution!
        int numDisplays = SDL_GetNumVideoDisplays();
        SDL_DisplayMode* currentDisplayMode = new SDL_DisplayMode[numDisplays];

        for (int k = 0; k < numDisplays; ++k)
        {
            int result = SDL_GetCurrentDisplayMode(k, &currentDisplayMode[k]);
        }

        // Use the widest display?
        int widest = 0;
        int andItsHeight = 0;

        for (int k = 0; k < numDisplays; ++k)
        {
            if (currentDisplayMode[k].w > widest)
            {
                widest = currentDisplayMode[k].w;
			
                andItsHeight = currentDisplayMode[k].h;
            }
        }
		
        delete[] currentDisplayMode;
        currentDisplayMode = 0;

        width = widest;
        height = andItsHeight;
    }
	   	   
    bool initialised = InitialiseOpenGL(width, height);

    SetFullscreen(!windowed);

    if (initialised)
    {
        m_pTextureManager = new TextureManager();
        assert(m_pTextureManager);
        initialised = m_pTextureManager->Initialise();
    }

	m_pCamera = camera;

#ifdef _DEBUG
	// Init ImGui
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(m_pWindow, m_glContext);
	ImGui_ImplOpenGL3_Init();
#endif // _DEBUG

	if (initialised)
	{
		LogManager::GetInstance().Log("Renderer Intialised");
	}
	else {
		LogManager::GetInstance().Error("Renderer failed to initialise!");
	}

    return initialised;
}

bool
Renderer::InitialiseOpenGL(int screenWidth, int screenHeight)
{
    m_iWidth = screenWidth;
    m_iHeight = screenHeight;

    m_pWindow = SDL_CreateWindow("Horde", SDL_WINDOWPOS_UNDEFINED, 
                                 SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    m_glContext = SDL_GL_CreateContext(m_pWindow);

    GLenum glewResult = glewInit();


    if (glewResult != GLEW_OK)
    {
        return false;
    }

    // Disable VSYNC
    SDL_GL_SetSwapInterval(0);

    bool shadersLoaded = SetupSpriteShader();

	TTF_Init();
	m_pFontSmall = TTF_OpenFont("assets\\fonts\\assistant-400.ttf", 18);
	m_pFontMedium = TTF_OpenFont("assets\\fonts\\assistant-400.ttf", 30);
	m_pFontLarge = TTF_OpenFont("assets\\fonts\\assistant-400.ttf", 60);

    return shadersLoaded;
}

void 
Renderer::Clear()
{
    glClearColor(m_fClearRed, m_fClearGreen, m_fClearBlue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

#ifdef _DEBUG
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_pWindow);
	ImGui::NewFrame();
#endif // _DEBUG
}

void 
Renderer::Present()
{
#ifdef _DEBUG
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif // _DEBUG

    SDL_GL_SwapWindow(m_pWindow);
}

void 
Renderer::SetFullscreen(bool fullscreen)
{
    if (fullscreen)
    {
        SDL_SetWindowFullscreen(m_pWindow, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_ALWAYS_ON_TOP);
        SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
        SDL_SetWindowSize(m_pWindow, m_iWidth, m_iHeight);
    }
    else
    {
        SDL_SetWindowFullscreen(m_pWindow, 0);
    }
}

void 
Renderer::SetClearColour(unsigned char r, unsigned char g, unsigned char b)
{
    m_fClearRed = r / 255.0f;
    m_fClearGreen = g / 255.0f;
    m_fClearBlue = b / 255.0f;
}

void 
Renderer::GetClearColour(unsigned char& r, unsigned char& g, unsigned char& b)
{
    r = static_cast<unsigned char>(m_fClearRed * 255.0f);
    g = static_cast<unsigned char>(m_fClearGreen * 255.0f);
    b = static_cast<unsigned char>(m_fClearBlue * 255.0f);
}

int 
Renderer::GetWidth() const
{
    return m_iWidth;
}

int 
Renderer::GetHeight() const
{
    return m_iHeight;
}

Sprite* 
Renderer::CreateSprite(const char* pcFilename, SpriteOrigin origin)
{
	assert(m_pTextureManager);

	Texture* pTexture = m_pTextureManager->GetTexture(pcFilename);

	std::vector<float> vertices;
	Sprite::GenerateVertices
	(
		&vertices,
		{ 0.f, 1.f }, 
		{ 1.f, 1.f }, 
		{ 1.f, 0.f }, 
		{ 0.f, 0.f }, 
		origin
	);

	unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

	VertexArray* spriteVertexData = new VertexArray(&vertices[0], 4, indices, 6);

    Sprite* pSprite = new Sprite();
    if (!pSprite->Initialise(*pTexture, *spriteVertexData))
    {
       LogManager::GetInstance().Log("Sprite Failed to Create!");
    }

    return (pSprite);
}

SpriteSheetAnimation*
Renderer::CreateAnimatedSprite(const char* pcFilename)
{
	assert(m_pTextureManager);
	Texture* pTexture = m_pTextureManager->GetTexture(pcFilename);
	SpriteSheetAnimation* pSprite = new SpriteSheetAnimation();
	if (!pSprite->Initialise(*pTexture))
	{
		LogManager::GetInstance().Log("SpriteSheetAnimation failed to create!");
	}
	return pSprite;
}

void 
Renderer::LogSdlError()
{
    LogManager::GetInstance().Log(SDL_GetError());
}

bool 
Renderer::SetupSpriteShader()
{
    m_pSpriteShader = new Shader();

    bool loaded = m_pSpriteShader->Load("shaders\\sprite.vert", "shaders\\sprite.frag");
    
    m_pSpriteShader->SetActive();

    return loaded;
}

void
Renderer::DrawSprite(Sprite& sprite, Vector2 position, float angle, DrawSpace drawSpace)
{
    m_pSpriteShader->SetActive();
    
    float sizeX = static_cast<float>(sprite.GetWidth());
    float sizeY = static_cast<float>(sprite.GetHeight());

	float drawX = position.x;
	float drawY = position.y;
		
	if (drawSpace == DrawSpace::WORLD_SPACE)
	{
		drawX = m_iWidth / 2.f + (Renderer::WorldSpaceToScreenSpace(m_pCamera->GetPosition().x + position.x));
		drawY = m_iHeight / 2.f + (Renderer::WorldSpaceToScreenSpace(m_pCamera->GetPosition().y - position.y));
	}
	
    Matrix4 world;
    SetIdentity(world);
    world.m[0][0] = cosf(angle) * (sizeX);
    world.m[0][1] = -sinf(angle) * (sizeX);
    world.m[1][0] = sinf(angle) * (sizeY);
    world.m[1][1] = cosf(angle) * (sizeY);
    world.m[3][0] = drawX;
    world.m[3][1] = drawY;

    m_pSpriteShader->SetMatrixUniform("uWorldTransform", world);

    Matrix4 orthoViewProj;
    CreateOrthoProjection(orthoViewProj, static_cast<float>(m_iWidth), static_cast<float>(m_iHeight));

    m_pSpriteShader->SetVector4Uniform("colour", sprite.GetRedTint(), 
                                                 sprite.GetGreenTint(), 
                                                 sprite.GetBlueTint(), 
                                                 sprite.GetAlpha());
	
    m_pSpriteShader->SetMatrixUniform("uViewProj", orthoViewProj);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


void
Renderer::DrawAnimatedSprite(SpriteSheetAnimation& sprite, Vector2& position, float angle, int frame, DrawSpace drawSpace)
{
	m_pSpriteShader->SetActive();

	float sizeX = static_cast<float>(sprite.GetWidth());
	float sizeY = static_cast<float>(sprite.GetHeight());

	float drawX = position.x;
	float drawY = position.y;

	if (drawSpace == DrawSpace::WORLD_SPACE)
	{
		drawX = m_iWidth / 2.f + (Renderer::WorldSpaceToScreenSpace(m_pCamera->GetPosition().x + position.x));
		drawY = m_iHeight / 2.f + (Renderer::WorldSpaceToScreenSpace(m_pCamera->GetPosition().y - position.y));
	}

	Matrix4 world;
	SetIdentity(world);
	world.m[0][0] = cosf(angle) * (sizeX);
	world.m[0][1] = -sinf(angle) * (sizeX);
	world.m[1][0] = sinf(angle) * (sizeY);
	world.m[1][1] = cosf(angle) * (sizeY);
	world.m[3][0] = drawX;
	world.m[3][1] = drawY;

	m_pSpriteShader->SetMatrixUniform("uWorldTransform", world);

	Matrix4 orthoViewProj;
	CreateOrthoProjection(orthoViewProj, static_cast<float>(m_iWidth), static_cast<float>(m_iHeight));
	m_pSpriteShader->SetVector4Uniform("colour", 
		sprite.GetRedTint(),
		sprite.GetGreenTint(), 
		sprite.GetBlueTint(), 
		sprite.GetAlpha());

	m_pSpriteShader->SetMatrixUniform("uViewProj", orthoViewProj);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)((frame * 6) * sizeof(GLuint)));
}

Sprite*
Renderer::CreateStaticTextSprite(const char* text, FontSize fSize, SDL_Color color, SpriteOrigin origin)
{
	Texture* pTexture = m_pTextureManager->GetTextTexture(text, GetFontFromEnum(fSize), color);
	return CreateSprite(text, origin);
}

// TODO: replace this with sprite sheet art, rather than dynamically swapping textures every single frame
void
Renderer::DrawDynamicText(const char* text, FontSize fSize, Vector2& position, SDL_Color color, SpriteOrigin origin)
{
	Sprite* spText = CreateStaticTextSprite(text, fSize, color, origin);

	spText->Draw(*this, position, 0.f, DrawSpace::SCREEN_SPACE);
	m_pTextureManager->RemoveTexture(text);

	delete spText;
	spText = NULL;
}

TTF_Font* 
Renderer::GetFontFromEnum(FontSize fSize)
{
	if (fSize == FontSize::SMALL)
	{
		return m_pFontSmall;
	}

	if (fSize == FontSize::MEDIUM)
	{
		return m_pFontMedium;
	}

	if (fSize == FontSize::LARGE)
	{
		return m_pFontLarge;
	}

	return NULL;
}

#ifdef _DEBUG
void 
Renderer::DebugDraw()
{
	m_pTextureManager->DebugDraw();
}
#endif // _DEBUG
