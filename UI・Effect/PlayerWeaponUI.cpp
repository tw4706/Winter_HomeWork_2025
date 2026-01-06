#include "PlayerWeaponUI.h"
#include "Player.h"
#include"GlobalConstants.h"
#include<Dxlib.h>

namespace
{

	//武器アイコンの拡大率
    constexpr float kWeaponScale = 1.0f;
	//アイコンフレームの拡大率
    constexpr float kFrameScale =2.0f;
	//武器の数
    constexpr int kWeaponNum = 3;

    //各武器画像
    const char* kWeaponGraphPath[kWeaponNum] =
    {
        "data/Bullet/Knife.png",
        "data/Bullet/Lance.png",
        "data/Bullet/Torch.png"
    };

	const char* kIconFrameGraphPath = "data/UI/WeaponUIFrame.png";

    constexpr int kIconSize = 64;

	//アイコン同士の間隔
    constexpr int kSpacing = 10;

    constexpr int kBasePosX = 20;
    constexpr int kBasePosY = 20;

    constexpr int kGrayAlpha = 100; // 非選択
    constexpr int kLockAlpha = 50;  // 未解放
}

PlayerWeaponUI::PlayerWeaponUI():
	currentWeapon_(BulletType::Knife),
	isTorchUnlocked_(false),
	iconFrameGraph_(-1)
{
}

PlayerWeaponUI::~PlayerWeaponUI()
{
    for(int graph : weaponGraphs_)
    {
        DeleteGraph(graph);
	}

	DeleteGraph(iconFrameGraph_);
}

void PlayerWeaponUI::Init()
{
    weaponGraphs_.resize(kWeaponNum);

    for (int i = 0; i < kWeaponNum; i++)
    {
        weaponGraphs_[i] = LoadGraph(kWeaponGraphPath[i]);
    }

	iconFrameGraph_ = LoadGraph(kIconFrameGraphPath);
}

void PlayerWeaponUI::Update(const Player&player)
{
    currentWeapon_ = player.GetCurrentBulletType();
    isTorchUnlocked_ = player.IsUnlockedTorch();
}

void PlayerWeaponUI::Draw()
{
    int index = static_cast<int>(currentWeapon_);

    // Torch が未解放なら表示しない（or ロック表示）
    if (currentWeapon_ == BulletType::Torch && !isTorchUnlocked_)
    {
        // 表示しない場合
        return;

        // ロック表示したいなら下のコメントアウトを使う
        /*
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, kLockAlpha);
        DrawGraph(kBasePosX, kBasePosY, weaponGraphs_[index], TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        return;
        */
    }

    int baseX = Game::kScreenWidth / 2 - kIconSize / 2;
    int baseY = 20;

    //中心座標
    int centerX = baseX + kIconSize / 2;
    int centerY = baseY + kIconSize / 2;

    //フレーム
    {
        int frameSize = static_cast<int>(kIconSize * kFrameScale);
        int half = frameSize / 2;

        DrawExtendGraph(
            centerX - half,
            centerY - half+10,
            centerX + half,
            centerY + half+10,
            iconFrameGraph_,
            TRUE);
    }

    //武器
    {
        int weaponSize = static_cast<int>(kIconSize * kWeaponScale);
        int half = weaponSize / 2;

        DrawExtendGraph(
            centerX - half,
            centerY - half+10,
            centerX + half,
            centerY + half+10,
            weaponGraphs_[index],
            TRUE);
    }
}
