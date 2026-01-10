#pragma once
#include"Geometry.h"
#include "BGMManager.h"

//アプリケーションを管理する
//シングルトンのクラス
class Application
{
private:
	Size windowSize_{};
	Application();
	Application(const Application& app) = delete;
	void operator=(const Application& app) = delete;

	//アプリケーション終了要求するフラグ
	bool requestedExit_ = false;

	BGMManager bgmManager_;//BGM管理クラス

public:
	~Application();
	/// <summary>
	/// Applicationのインスタンスを取得する
	/// </summary>
	/// <returns></returns>
	/// <note>Applicationの実態はこの関数内部で
	static Application& GetInstance();

	//初期化
	bool Init();
	//更新処理
	void Run();
	void Terminate();

	//ウィンドウサイズを返す関数
	const Size& GetWindowSize()const;

	//アプリケーションの終了を要求する関数
	void RequestExit();

	//BGMManagerを取得する関数
	BGMManager& GetBGMManager() { return bgmManager_; }
};

