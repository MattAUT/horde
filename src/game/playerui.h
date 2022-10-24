#ifndef __PLAYERUI_H__
#define __PLAYERUI_H__

class Sprite;
class SpriteSheetAnimation;
class Renderer;
class Player;

#include <vector2.h>
#include <tuple>

class PlayerUI
{
public:
	PlayerUI();
	~PlayerUI();
	bool Initialise(Renderer& renderer, Player& player);
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	void DoDamageFlash();

protected:
private:

public:
protected:
	Sprite* m_pOverheatBar;
	Sprite* m_pOverheatBarUnderlay;
	Sprite* m_pHealthBar;
	Sprite* m_pHealthBarUnderlay;
	Sprite* m_pHealthIcon;
	Sprite* m_pWeaponIcon;
	SpriteSheetAnimation* m_pDamageFlash;
	Player* m_pPlayer;

private:

};

#endif // __PLAYERUI_H__