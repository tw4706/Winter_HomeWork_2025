#pragma once
class SceneController;
class Input;
/// <summary>
/// シーン基底クラス
/// </summary>
class Scene
{
protected:
	SceneController& controller_;
public:
	Scene(SceneController& controller);

	/// <summary>
	/// シーンの情報の更新
	/// </summary>
	/// <param name="input"></param>
	virtual void Update(Input& input) = 0;

	/// <summary>
	/// シーンの描画
	/// </summary>
	virtual void Draw() = 0;
};

