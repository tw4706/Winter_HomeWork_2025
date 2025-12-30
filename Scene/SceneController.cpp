#include "SceneController.h"
#include"Scene.h"


void SceneController::ResetScene(std::shared_ptr<Scene> scene)
{
	if (!scene) return;
	scenes_.clear();
	scenes_.push_back(scene);
	scene->Init();
}

void SceneController::ChangeScene(std::shared_ptr<Scene> scene)
{

    if (!scene) return;

    if (!scenes_.empty()) 
	{
		scenes_.pop_back(); //古いシーンを削除
    }
    scenes_.push_back(scene); //新しいシーンを追加

	scene->Init();

}

void SceneController::PushScene(std::shared_ptr<Scene> scene)
{
	//新しいシーンを末尾に積みます
	if (!scene) return;
	scenes_.push_back(scene);
	scene->Init();
}

void SceneController::PopScene()
{
	//末尾のシーンを削除します
	if (scenes_.empty()) return;
	scenes_.pop_back();
}

void SceneController::Update(Input& input)
{
	//末尾の要素に対してのみUpdateする
	if (!scenes_.empty())
	{
		scenes_.back()->Update(input);
	}
}

void SceneController::Draw()
{
	//下から順にすべてのシーンを描画する
	for (auto& scene : scenes_)
	{
		scene->Draw();
	}
}
