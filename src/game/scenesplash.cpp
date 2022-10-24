// COMP710 GP Framework 2022

#include "scenesplash.h"

#include <engine/renderer.h>
#include <engine/sprite.h>
#include <engine/scenemanager.h>
#include <engine/input.h>

#include <cassert>
#include <utils.h>
#include <cmath>
#include <imgui/imgui_impl_sdl.h>

SceneSplash::SceneSplash()
	: m_fTime(-0.4f) 
	, m_fSceneLength(10)
{
#ifdef _DEBUG
	dm_name = "Splash Screen Scene";
#endif // _DEBUG
}

SceneSplash::~SceneSplash()
{
	Deinitialise();
}

bool SceneSplash::Initialise(Renderer& renderer, Camera& camera, SceneManager& sceneManager, FMOD::System& fmodSystem)
{
	m_pAUTlogo = renderer.CreateSprite("assets\\sprites\\logos\\aut.png", SpriteOrigin::CENTER);
	m_pAUTlogo->SetScale(0.4f);
	m_pAUTlogo->SetAlpha(0.f);

	m_pFMODlogo = renderer.CreateSprite("assets\\sprites\\logos\\fmod.png", SpriteOrigin::CENTER);
	m_pFMODlogo->SetScale(0.4f);

	m_pSDLlogo = renderer.CreateSprite("assets\\sprites\\logos\\sdl.jpg", SpriteOrigin::CENTER);
	m_pSDLlogo->SetScale(0.5f);

	m_pOPENGLlogo = renderer.CreateSprite("assets\\sprites\\logos\\opengl.png", SpriteOrigin::CENTER);
	m_pOPENGLlogo->SetScale(0.3f);

	m_pBOX2Dlogo =  renderer.CreateSprite("assets\\sprites\\logos\\box2d.png", SpriteOrigin::CENTER);
	m_pBOX2Dlogo->SetScale(0.2f);

	return Scene::Initialise(renderer, camera, sceneManager, fmodSystem);
}

void
SceneSplash::Deinitialise()
{
	m_fSceneLength = 0.0f;

	delete m_pAUTlogo;
	delete m_pFMODlogo;
	delete m_pSDLlogo;
	delete m_pOPENGLlogo;
	delete m_pBOX2Dlogo;

	m_pAUTlogo = nullptr;
	m_pFMODlogo = nullptr;
	m_pSDLlogo = nullptr;
	m_pOPENGLlogo = nullptr;
	m_pBOX2Dlogo = nullptr;

	Scene::Deinitialise();
}

void SceneSplash::Process(float deltaTime, Input& input)
{
	m_fTime += deltaTime;

	if (input.GetKeyState(SDL_SCANCODE_SPACE) == BS_PRESSED)
	{
		Skip();
	}

	float autAlpha = EaseInOutQuad(m_fTime - m_fSceneLength / 2.f, 0, 1, m_fSceneLength / 4.f);
	m_pAUTlogo->SetAlpha(autAlpha);

	float libAlpha = EaseInOutQuad(m_fTime, 0, 1, m_fSceneLength / 4.f);
	m_pFMODlogo->SetAlpha(libAlpha);
	m_pSDLlogo->SetAlpha(libAlpha);
	m_pOPENGLlogo->SetAlpha(libAlpha);
	m_pBOX2Dlogo->SetAlpha(libAlpha);

	if (m_fTime > m_fSceneLength)
	{
		m_pSceneManager->SwitchScenes(1);
	}
}

void SceneSplash::Draw(Renderer& renderer)
{
	const int SCREEN_WIDTH = renderer.GetWidth();
	const int SCREEN_HEIGHT = renderer.GetHeight();

	Vector2 logoAUTpos = { SCREEN_WIDTH / 2.f , SCREEN_HEIGHT / 2.f };
	m_pAUTlogo->Draw(renderer, logoAUTpos, 0.f, DrawSpace::SCREEN_SPACE);


	Vector2 logoFMODpos = { SCREEN_WIDTH / 2.1f - (m_pFMODlogo->GetWidth() / 2.f) - 30 , SCREEN_HEIGHT / 2.f - 100 };
	m_pFMODlogo->Draw(renderer, logoFMODpos, 0.f, DrawSpace::SCREEN_SPACE);

	Vector2 logoSDLDpos = { SCREEN_WIDTH / 2.1f - (m_pFMODlogo->GetWidth() / 2.f) - 30 , SCREEN_HEIGHT / 2.f + 100 };
	m_pSDLlogo->Draw(renderer, logoSDLDpos, 0.f, DrawSpace::SCREEN_SPACE);

	Vector2 logoOPENGLpos = { SCREEN_WIDTH - (SCREEN_WIDTH / 2.1f) + (m_pOPENGLlogo->GetWidth() / 2.f) - 30, SCREEN_HEIGHT / 2.f - 100 };
	m_pOPENGLlogo->Draw(renderer, logoOPENGLpos, 0.f, DrawSpace::SCREEN_SPACE);

	Vector2 logoBOX2Dpos = { SCREEN_WIDTH - (SCREEN_WIDTH / 2.1f) + (m_pOPENGLlogo->GetWidth() / 2.f) - 30, SCREEN_HEIGHT / 2.f + 80 };
	m_pBOX2Dlogo->Draw(renderer, logoBOX2Dpos, 0.f, DrawSpace::SCREEN_SPACE);
}

void
SceneSplash::Reset()
{

}

#ifdef _DEBUG
void SceneSplash::DebugDraw(Renderer& renderer)
{
	Scene::BeginDebugDraw();
	if (ImGui::Button("Reset Scene"))
	{
		m_fTime = 0;
	}

	if (ImGui::Button("Skip"))
	{
		Skip();
	}

	ImGui::SliderFloat("Time", &m_fTime, 0.f, m_fSceneLength);
	ImGui::SliderFloat("Scene Length", &m_fSceneLength, 0.f, 20.f);

	ImGui::End();
}
#endif // _DEBUG

void
SceneSplash::Skip()
{
	m_fTime = m_fSceneLength;
}