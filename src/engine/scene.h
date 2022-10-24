// COMP710 GP Framework 2022
#ifndef __SCENE_H__
#define __SCENE_H__

// Forward Declarations:
class Renderer;
class Camera;
class Input;
class SceneManager;
class Entity;

namespace FMOD
{
	class System;
}

#include <vector>

typedef std::vector<Entity*>::iterator EntityVectorIterator;

class Scene
{
    // Member Methods:
public:
	Scene();
	virtual ~Scene();

	virtual bool Initialise(Renderer& renderer, Camera& camera, SceneManager& sceneManager, FMOD::System& fmodSystem);
	// Deinitialise should be called during object destructuring. All destructuring 
	// methods should be placed in the Deinitialise method.
	virtual void Deinitialise();
	virtual void Process(float deltaTime, Input& input) = 0;
	virtual void Draw(Renderer& renderer) = 0;
	virtual void Reset() = 0;
#ifdef _DEBUG
	virtual void DebugDraw(Renderer& renderer) = 0;
	// Any ImGui elements can be appened to the end of this method
	// call, as long as ImGui::End is called sometime after. 
	virtual void BeginDebugDraw();
	void AddEntityToDebugQueue(Entity* entity);
	void RemoveEntityFromDebugQueue(Entity* rmEntity);
#endif // _DEBUG

	bool IsLoaded();

protected:
private:
    Scene(const Scene& scene);
	Scene& operator=(const Scene& scene) {}

    // Member Data:
public:
protected:
	SceneManager* m_pSceneManager;
	Renderer* m_pRenderer;
	Camera* m_pCamera;
	FMOD::System* m_pFMODsystem;

	bool m_bIsLoaded;

#ifdef _DEBUG
	const char* dm_name;
	int dm_iDisplayEntityIndex;
	std::vector<Entity*> dm_pEntityDebugQueue;
#endif // _DEBUG

private:

};

#endif // __SCENE_H__
