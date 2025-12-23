#pragma once
#include "GameProgress.h"
#include<memory>
#include<list>
#include <type_traits>

class Input;
class Scene;
class SceneController
{
private:
	//現在スタックに積まれているシーン
	//最後に積んだものだけがUpdateされる
	std::list<std::shared_ptr<Scene>>scenes_;

	GameProgress progress_;

public:

	//ゲームの進行状況を取得する関数
	GameProgress& GetProgress() { return progress_; }

	void ResetScene(std::shared_ptr<Scene>scene);

	/// <summary>
	/// シーンの切り替え
	/// </summary>
	/// <param name="scene">新しいシーン</param>
	/// <note>最後に積んだシーンがChangeされる</note>
	void ChangeScene(std::shared_ptr<Scene>scene);

	//シーンを積む
	void PushScene(std::shared_ptr<Scene>scene);

	//最後に積んだシーンをリストから取り除く
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

	// 追加: 型Tのシーンを取得する
	template<typename T>
	std::shared_ptr<T> GetScene()
	{
		for (auto it = scenes_.rbegin(); it != scenes_.rend(); ++it)
		{
			auto casted = std::dynamic_pointer_cast<T>(*it);
			if (casted)
				return casted;
		}
		return nullptr;
	}

};

