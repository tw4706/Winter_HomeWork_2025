#include<Dxlib.h>
#include<memory>
#include"Input.h"
#include "Application.h"
#include "Animation.h"
#include "GlobalConstants.h"
#include"SceneController.h"
#include"TitleScene.h"
#include "EffekseerForDXLib.h"

Application::Application():
	windowSize_(Game::kScreenWidth, Game::kScreenHeight)
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
	DxLib::ChangeWindowMode(false);
	// ウインドウのタイトル変更
	DxLib::SetWindowText("VillageMonster");
	// 画面のサイズ変更
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorBitNum);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return false;			// エラーが起きたら直ちに終了
	}

	// グラフィックの描画先を裏画面にセット
	SetDrawScreen(DX_SCREEN_BACK);

	//------------------------------//
	// エフェクトの初期化
	//------------------------------//
	{
		// DirectX9を使用するようにする。(DirectX11も可)
		// Effekseerを使用するには必ず設定する。
		SetUseDirect3DVersion(DX_DIRECT3D_11);

		// Effekseerを初期化する。
		// 引数には画面に表示する最大パーティクル数を設定する。
		if (Effkseer_Init(8000) == -1)
		{
			DxLib_End();
			return false;
		}

		// フルスクリーンウインドウの切り替えでリソースが消えるのを防ぐ。
		// Effekseerを使用する場合は必ず設定する。
		SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

		// DXライブラリのデバイスロストした時のコールバックを設定する。
		// ウインドウとフルスクリーンの切り替えが発生する場合は必ず実行する。
		// ただし、DirectX11を使用する場合は実行する必要はない。
		Effekseer_SetGraphicsDeviceLostCallbackFunctions();

		// Effekseerに2D描画の設定をする。
		Effekseer_Set2DSetting(Game::kScreenWidth, Game::kScreenHeight);

		// Zバッファを有効にする。
		// Effekseerを使用する場合、2DゲームでもZバッファを使用する。
		SetUseZBuffer3D(TRUE);

		// Zバッファへの書き込みを有効にする。
		// Effekseerを使用する場合、2DゲームでもZバッファを使用する。
		SetWriteZBuffer3D(TRUE);
	}

	AddFontResourceEx("data/UI/g_comichorrorB_freeR.ttf", FR_PRIVATE, NULL);

	//ここでBGMManagerの初期化を行う
	bgmManager_.Init();
	seManager_.Init();

	return true;
}

void Application::Run()
{
	// 描画対象をバックバッファに変更
	SetDrawScreen(DX_SCREEN_BACK);

	Input input;
	SceneController controller;
	controller.ChangeScene(std::make_shared<TitleScene>(controller));

	while (ProcessMessage() != -1 && !requestedExit_)
	{
		// このフレームの開始時間を取得
		LONGLONG start = GetNowHiPerformanceCount();

		// 前のフレームに描画した内容をクリアする
		ClearDrawScreen();

		// ここにゲームの処理を書く
		input.Update();
		controller.Update(input);

		controller.Draw();

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
	RemoveFontResourceEx("data/UI/g_comichorrorB_freeR.ttf", FR_PRIVATE, NULL);
	Effkseer_End();
	DxLib_End();				//ＤＸライブラリ使用の終了処理
}

const Size& Application::GetWindowSize() const
{
	return windowSize_;
}

void Application::RequestExit()
{
	requestedExit_ = true;
}
