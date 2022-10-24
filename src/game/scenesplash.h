#ifndef __SCENESPLASH_H__
#define __SCENESPLASH_H__

#include <engine/scene.h>

// Forward Declarations
class SceneManager;
class Sprite;

namespace FMOD
{
	class System;
}

class SceneSplash : public Scene
{
	// Member Methods
public:
	SceneSplash();
	virtual ~SceneSplash();

	virtual bool Initialise(Renderer& renderer, Camera& camera, SceneManager& sceneManager, FMOD::System& fmodSystem) override;
	virtual void Deinitialise() override;
	virtual void Process(float deltaTime, Input& input) override;
	virtual void Draw(Renderer& renderer) override;
	virtual void Reset() override;
#ifdef _DEBUG
	virtual void DebugDraw(Renderer& renderer) override;
#endif // _DEBUG

protected:
	void Skip();

private:
	SceneSplash(const SceneSplash& s);
	SceneSplash& operator=(const SceneSplash& s) {}

	// Member Data
public:
protected:
	Sprite* m_pAUTlogo;
	Sprite* m_pSDLlogo;
	Sprite* m_pFMODlogo;
	Sprite* m_pOPENGLlogo;
	Sprite* m_pBOX2Dlogo;

	float m_fTime; 
	float m_fSceneLength; // Amount of time the splash screen will last
private:

};

#endif // __SCENESPLASH_H__

