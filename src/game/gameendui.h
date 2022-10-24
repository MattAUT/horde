#ifndef __GAMEENDUI_H__
#define __GAMEENDUI_H__

class Sprite;
class SpriteSheetAnimation;
class Renderer;
class Player;


struct Statistics
{
	unsigned int round;
	unsigned int killCount;
	float playTime;
};

class GameEndUI
{
public:
	GameEndUI();
	~GameEndUI();
	bool Initialise(Renderer& renderer);
	bool Setup(Statistics stats);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	bool IsLoaded() const;

protected:
private:
	GameEndUI(const GameEndUI& g);
	GameEndUI& operator=(const GameEndUI& g) {}

public:
protected:
	bool m_bIsLoaded;
	bool m_bDrawText;
	Sprite* m_pBackground;
	Sprite* m_pForeground;
	Sprite* m_pGameOverText;
	Sprite* m_pActionText;
	char m_sRoundText[32];
	char m_sTimeText[32];
	char m_sKillsText[32];
private:

};

#endif // __GAMEENDUI_H__