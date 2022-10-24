#ifndef __PAUSEUI_H__
#define __PAUSEUI_H__

class Sprite;
class Renderer;
class Input;
class SceneLevel;

namespace FMOD
{
	class System;
	class Sound;
	class Channel;
}

class PauseUI
{
public:
	PauseUI();
	~PauseUI();
	bool Initialise(Renderer& renderer, SceneLevel& scene, FMOD::System& fmodSystem);
	void Process(float deltaTime, Input& input);
	void Draw(Renderer& renderer);
	bool IsLoaded() const;

protected:
private:
	PauseUI(const PauseUI& g);
	PauseUI& operator=(const PauseUI& g) {}

public:
protected:
	SceneLevel* m_pSceneLevel;
	FMOD::System* m_pFMODsystem;

	Sprite* m_pBackground;
	Sprite* m_pHeader;
	Sprite* m_pResumeText;
	Sprite* m_pExitText;

	bool m_bSelectedExit;
	Sprite* m_pSelector;
	float m_fTime;

	FMOD::Sound* m_pSelectSound;
	FMOD::Channel* m_pSelectSoundChannel;

private:

};

#endif // __PAUSEUI_H__