#include<Dxlib.h>
#include<memory>
#include "Application.h"
#include "Game.h"
#include"Input.h"
#include"Player.h"
#include"Zombie.h"
#include"SceneController.h"
#include"TitleScene.h"

Application::Application()
{

}


Application::~Application()
{
}

Application& Application::GetInstance()
{
	static Application instance;
	return instance;
}

bool Application::Init()
{
	DxLib::ChangeWindowMode(true);
	// ウインドウのタイトル変更
	DxLib::SetWindowText("VillageMonster");
	// 画面のサイズ変更
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorBitNum);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return false;			// エラーが起きたら直ちに終了
	}
	else
	{
		return true;
	}
}

void Application::Run()
{
	// 描画対象をバックバッファに変更
	SetDrawScreen(DX_SCREEN_BACK);
	Input input;
	Player player({ 100, 500 }, Vector2());
	Zombie zombie({ 800,500 });
	player.Init();
	zombie.Init();

	while (ProcessMessage() != -1)
	{
		// このフレームの開始時間を取得
		LONGLONG start = GetNowHiPerformanceCount();

		// 前のフレームに描画した内容をクリアする
		ClearDrawScreen();

		// ここにゲームの処理を書く
		input.Update();
		player.Update(input);
		zombie.Update();
		player.Draw();
		zombie.Draw();

		// escキーを押したらゲームを強制終了
		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}

		// 描画した内容を画面に反映する
		ScreenFlip();

		// フレームレート60に固定
		while (GetNowHiPerformanceCount() - start < 16667)
		{

		}
	}
}

void Application::Terminate()
{
	DxLib_End();				// ＤＸライブラリ使用の終了処理
}
