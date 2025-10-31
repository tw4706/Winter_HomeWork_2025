#pragma once
#include<memory>
#include<list>
class Input;
class Scene;
class SceneController
{
private:
	//現在スタックに積まれているシーン
	//最後に積んだものだけがUpdateされる
	std::list<std::shared_ptr<Scene>>scenes_;
public:
	/// <summary>
	/// シーンの切り替え
	/// </summary>
	/// <param name="scene">新しいシーン</param>
	/// <note>最後に積んだシーンがChangeされる</note>
	void ChangeScene(std::shared_ptr<Scene>scene);

	/// <summary>
	/// シーンを新しく積みます
	/// </summary>
	/// <param name="scene">積みたいシーン</param>
	/// <note>ChangeSceneと違って、もともと積まれているシーンは残り続ける
	void PushScene(std::shared_ptr<Scene>scene);

	/// <summary>
	/// 最後に積んだシーンをリストから外す(自動的に解放される)
	/// </summary>
	void PopScene();

	/// <summary>
	/// 内部に持っているシーンのUpdateを呼び出す
	/// </summary>
	/// <param name="input"></param>
	void Update(Input& input);

	/// <summary>
	/// 内部に持っているDrawを呼び出す
	/// </summary>
	void Draw();
};

