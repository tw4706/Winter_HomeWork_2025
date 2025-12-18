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
	bool requestedExit_ = false;
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

	void RequestExit();
};

