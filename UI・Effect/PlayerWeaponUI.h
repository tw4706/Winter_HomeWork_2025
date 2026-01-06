#pragma once
#include <vector>
#include"Bullet.h"

class Player;
class PlayerWeaponUI
{
public:
	PlayerWeaponUI();
	~PlayerWeaponUI();

	void Init();
	void Update(const Player& player);
	void Draw();
private:
	std::vector<int>weaponGraphs_;//武器の画像ハンドル
	BulletType currentWeapon_;//現在の武器
	bool isTorchUnlocked_ = false;//たいまつ解放フラグ

	int iconFrameGraph_;//武器のフレームの画像ハンドル
};

