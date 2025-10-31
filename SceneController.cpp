#include "SceneController.h"
#include"Scene.h"


void SceneController::ChangeScene(std::shared_ptr<Scene> scene)
{
	if (scenes_.empty())
	{
		scenes_.push_back(scene);
	}
	else
	{
		scenes_.back() = scene;
	}
}

void SceneController::PushScene(std::shared_ptr<Scene> scene)
{
	scenes_.push_back(scene);
}

void SceneController::PopScene()
{
	scenes_.pop_back();
}

void SceneController::Update(Input&input)
{
	//––”ö‚Ì—v‘f‚É‘Î‚µ‚Ä‚Ì‚ÝUpdate‚·‚é
	scenes_.back()->Update(input);
}

void SceneController::Draw()
{
	//––”ö‚Ì—v‘f‚É‘Î‚µ‚Ä‚Ì‚ÝUpdate‚·‚é
	for(auto& scene:scenes_)
	{
		scene->Draw();
	}
}
