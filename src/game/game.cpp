// COMP710 GP Framework 2022
#include "game.h"

#include <engine/renderer.h>
#include <engine/camera.h>
#include <engine/scene.h>
#include <engine/scenemanager.h>
#include <engine/input.h>
#include <engine/xinputcontroller.h>
#include <fmod.hpp>

#include <cstdio>
#include <cassert>
#include <ctime>
#include <logmanager.h>
#include <ini.h>
#ifdef _DEBUG
#include <imgui/imgui_impl_sdl.h>
#endif // _DEBUG

// Scenes
#include "scenesplash.h"
#include "scenemenu.h"
#include "scenelevel.h"

// Static Members:
Game *Game::sm_pInstance = 0;

Game &Game::GetInstance() {
    if (sm_pInstance == 0) {
        sm_pInstance = new Game();
    }

    return (*sm_pInstance);
}

void Game::DestroyInstance() {
    delete sm_pInstance;
    sm_pInstance = 0;
}

Game::Game()
    : m_pRenderer(0)
	, m_bLooping(true)
	, m_fExecutionTime(0),
      m_fElapsedSeconds(0)
	, m_iFrameCount(0)
	, m_fFrametimeUpdate(0.25f)
	, m_bShowDebugWindow(false)
{

}

Game::~Game() {
	m_pFMODsystem->release();

    delete m_pRenderer;
	delete m_pCamera;
    delete m_pInput;
	delete m_pSceneManager;

    m_pRenderer = nullptr;
    m_pInput = nullptr;
	m_pSceneManager = nullptr;
}

void 
Game::Quit() 
{ 
	m_bLooping = false; 
}

void
Game::ToggleDebugWindow() 
{
    m_bShowDebugWindow = !m_bShowDebugWindow;
    m_pInput->ShowMouseCursor(m_bShowDebugWindow);
}

bool 
Game::Initialise()
{
	srand(static_cast<unsigned int>(time(NULL)));

    int bbWidth = 1280;
    int bbHeight = 960;

	m_pCamera = new Camera();
    m_pRenderer = new Renderer();
    if (!m_pRenderer->Initialise(false, m_pCamera, bbWidth, bbHeight))
	{
        LogManager::GetInstance().Error("Renderer failed to initialise!");
        return false;
    }

    bbWidth = m_pRenderer->GetWidth();
    bbHeight = m_pRenderer->GetHeight();
    m_iLastTime = SDL_GetPerformanceCounter();
    m_pRenderer->SetClearColour(0, 0, 0);

    m_pInput = new Input();
    m_pInput->Initialise();

	if (FMOD::System_Create(&m_pFMODsystem) != FMOD_OK)
	{
		LogManager::GetInstance().Error("Could not create FMOD instance!");
		assert(0);
	}

	if (m_pFMODsystem->init(512, FMOD_INIT_NORMAL, NULL))
	{
		LogManager::GetInstance().Error("FMOD failed to initalise!");
		assert(0);
	}

	assert(m_pFMODsystem);
	LogManager::GetInstance().Log("FMOD Initialised.");

	m_pSceneManager = new SceneManager();
	Scene* scenes[SceneManager::sm_uiSceneCount] = { new SceneSplash(), new SceneMenu(), new SceneLevel() };
	m_pSceneManager->Initialise(*this, scenes, *m_pCamera, *m_pRenderer, *m_pFMODsystem);
	m_pSceneManager->SwitchScenes(0);

    return true;
}

bool 
Game::DoGameLoop() 
{
    const float stepSize = 1.0f / 200.0f;

    m_pInput->ProcessInput();
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
#ifdef _DEBUG
        ImGuiIO &io = ImGui::GetIO();
        ImGui_ImplSDL2_ProcessEvent(&event);
#endif // _DEBUG
    }

	XInputController *controller = m_pInput->GetController(0);

    if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
        Quit();
    }
#ifdef _DEBUG
	bool debugKeyPressed =
		m_pInput->GetKeyState(SDL_SCANCODE_GRAVE) == BS_PRESSED;

	if (debugKeyPressed) {
		ToggleDebugWindow();
	}
#endif // _DEBUG

    if (m_bLooping) {
        Uint64 current = SDL_GetPerformanceCounter();
        float deltaTime = (current - m_iLastTime) /
                          static_cast<float>(SDL_GetPerformanceFrequency());
        m_iLastTime = current;

        m_fExecutionTime += deltaTime;

        Process(deltaTime);

#ifdef USE_LAG
        m_fLag += deltaTime;

        int innerLag = 0;

        while (m_fLag >= stepSize) {
            Process(stepSize);

            m_fLag -= stepSize;

            ++m_iUpdateCount;
            ++innerLag;
        }
#endif // USE_LAG

		ProcessFrameCounting(deltaTime);
        Draw(*m_pRenderer);
		m_pSceneManager->GetActiveScene()->Reset();
    }

    return m_bLooping;
}

void 
Game::Process(float deltaTime) 
{
    
    m_pSceneManager->GetActiveScene()->Process(deltaTime, *m_pInput);
	m_pFMODsystem->update();
}

void
Game::Draw(Renderer &renderer) 
{
    ++m_iFrameCount;

    renderer.Clear();

	m_pSceneManager->GetActiveScene()->Draw(renderer);

#ifdef _DEBUG
    DebugDraw(renderer);
#endif // _DEBUG

    renderer.Present();
}

#ifdef _DEBUG
void 
Game::DebugDraw(Renderer &renderer)
{
    if (m_bShowDebugWindow) {
		// On Screen Text
        char fpsText[16];
		Vector2 fpsPosition = { 6, 0 };
        sprintf_s(fpsText, "FPS: %d", m_iFPS);
        renderer.DrawDynamicText(fpsText, FontSize::SMALL, fpsPosition, {255, 255, 255, 255}, SpriteOrigin::TOP_LEFT);

		char frameTimeText[32];
		Vector2 frameTimePosition = { 6, 24 };
		sprintf_s(frameTimeText, "Frame Times: %fms", 1.f / m_iFPS);
		renderer.DrawDynamicText(frameTimeText, FontSize::SMALL, frameTimePosition, { 255, 255, 255, 255 }, SpriteOrigin::TOP_LEFT);

		// ImGui
        bool open = true;
        ImGui::Begin("Application", &open, ImGuiWindowFlags_MenuBar);
        ImGui::Text("Engine Framework (COMP710)");
        if (ImGui::Button("Quit")) {
            Quit();
        }

		ImGui::Separator();
		ImGui::Spacing();
		LogManager::GetInstance().DebugDraw();

        ImGui::End();

		m_pRenderer->DebugDraw();
		m_pSceneManager->GetActiveScene()->DebugDraw(renderer);
    }
}
#endif // _DEBUG

void 
Game::ProcessFrameCounting(float deltaTime) 
{
    // Count total simulation time elapsed:
    m_fElapsedSeconds += deltaTime;

    // Frame Counter:
    if (m_fElapsedSeconds > m_fFrametimeUpdate) {
        m_fElapsedSeconds -= m_fFrametimeUpdate;
        m_iFPS = static_cast<int>(m_iFrameCount / m_fFrametimeUpdate);
        m_iFrameCount = 0;
    }
}
