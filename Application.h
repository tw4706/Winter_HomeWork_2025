#pragma once
#include"Geometry.h"
//アプリケーションを管理する
//シングルトンのクラス
class Application
{
private:
	Size windowSize_{};
	Application();
	Application(const Application& app) = delete;
	void operator=(const Application& app) = delete;
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

	//ウィンドウサイズの取得
	const Size& GetWindowSize()const { return windowSize_; }
};

