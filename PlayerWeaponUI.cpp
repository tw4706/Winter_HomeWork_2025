#include "PlayerWeaponUI.h"
#include "Player.h"
#include<Dxlib.h>

namespace
{
	//武器の数
    constexpr int kWeaponNum = 3;

    //各武器画像
    const char* kWeaponGraphPath[kWeaponNum] =
    {
        "data/Bullet/Knife.png",
        "data/Bullet/Lance.png",
        "data/Bullet/Torch.png"
    };

    constexpr int kIconSize = 64;

	//アイコン同士の間隔
    constexpr int kSpacing = 10;

    constexpr int kBasePosX = 20;
    constexpr int kBasePosY = 20;

    constexpr int kGrayAlpha = 100; // 非選択
    constexpr int kLockAlpha = 50;  // 未解放
}

PlayerWeaponUI::PlayerWeaponUI():
	currentWeapon_(BulletType::Knife)
{
}

PlayerWeaponUI::~PlayerWeaponUI()
{
    for(int graph : weaponGraphs_)
    {
        DeleteGraph(graph);
	}
}

void PlayerWeaponUI::Init()
{
    weaponGraphs_.resize(kWeaponNum);

    for (int i = 0; i < kWeaponNum; i++)
    {
        weaponGraphs_[i] = LoadGraph(kWeaponGraphPath[i]);
    }
}

void PlayerWeaponUI::Update(const Player&player)
{
    currentWeapon_ = player.GetCurrentBulletType();
    isTorchUnlocked_ = player.IsUnlockedTorch();
}

void PlayerWeaponUI::Draw()
{
    for (int i = 0; i < kWeaponNum; i++)
    {
        int x = kBasePosX + i * (kIconSize + kSpacing);
        int y = kBasePosY;

        bool isSelected = (i == static_cast<int>(currentWeapon_));
        bool isLocked =
            (i == static_cast<int>(BulletType::Torch)) && !isTorchUnlocked_;

        if (isLocked)
        {
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, kLockAlpha);
        }
        else if (!isSelected)
        {
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, kGrayAlpha);
        }
        else
        {
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }

        DrawGraph(x, y, weaponGraphs_[i], TRUE);
    }

    // 忘れず戻す
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
