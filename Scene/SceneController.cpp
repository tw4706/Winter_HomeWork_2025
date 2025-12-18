#include "SceneController.h"
#include"Scene.h"


void SceneController::ResetScene(std::shared_ptr<Scene> scene)
{
	if (!scene) return;
	scenes_.clear(); //全てのシーンを削除
	scenes_.push_back(scene); //新しいシーンを追加
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
	if (!scene) return;
	scenes_.push_back(scene); //新しいシーンを追加
	scene->Init();
}

void SceneController::PopScene()
{
	if (!scenes_.empty()) 
	{
		scenes_.pop_back(); //古いシーンを削除
	}
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
	//末尾の要素に対してのみUpdateする
	for (auto& scene : scenes_)
	{
		scene->Draw();
	}
}
