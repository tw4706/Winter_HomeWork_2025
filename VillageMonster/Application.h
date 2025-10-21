#pragma once
//アプリケーションを管理する
//シングルトンのクラス

class Application
{
private:
	Application();
	Application(const Application& app) = delete;
	void operator=(const Application& app) = delete;
public:
	~Application();
	static Application& GetInstance();

	//初期化
	bool Init();
	//更新処理
	void Run();
	void Terminate();
};

