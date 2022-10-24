#ifndef __SCENEMENU_H__
#define __SCENEMENU_H__

#include <engine/scene.h>
#include <vector2.h>

// Forward Declarations
class SceneManager;
class Sprite;

namespace FMOD
{
	class System;
	class Sound;
	class Channel;
}

namespace MENU
{
	enum State
	{
		MAIN_MENU,
		LEVEL_SELECT
	};
}

class SceneMenu : public Scene
{

	// Member Methods
public:
	SceneMenu();
	virtual ~SceneMenu();

	virtual bool Initialise(Renderer& renderer, Camera& camera, SceneManager& sceneManager, FMOD::System& fmodSystem) override;
	virtual void Deinitialise() override;
	virtual void Process(float deltaTime, Input& input) override;
	virtual void Draw(Renderer& renderer) override;
	virtual void Reset() override;
#ifdef _DEBUG
	virtual void DebugDraw(Renderer& renderer) override;
#endif // _DEBUG

protected:

private:
	SceneMenu(const SceneMenu& s);
	SceneMenu& operator=(const SceneMenu& s) {}

	// Member Data
public:
protected:
	Sprite* m_pHeader;
	Sprite* m_pPlayText;
	Sprite* m_pExitText;
	Sprite* m_pLVL1Thumb;
	Sprite* m_pLVL2Thumb;

	bool m_bSelectedExit;
	Sprite* m_pSelector;
	
	MENU::State m_state;

	float m_fTime;

	FMOD::Sound* m_pSelectSound;
	FMOD::Channel* m_pSelectSoundChannel;

private:

};

#endif // __SCENEMENU_H__

