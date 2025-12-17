#include "PauseScene.h"
#include "SceneController.h"

PauseScene::PauseScene(SceneController& controller):
	Scene(controller),
	frame_(0),
	selectIdx_(0),
	yesNoDialogSelectIdx_(1)
{
	update_ = &PauseScene::AppearUpdate;
	draw_ = &PauseScene::IntervalDraw;
}

void PauseScene::Update(Input& input)
{
	(this->*update_)(input);
}

void PauseScene::Draw()
{
	(this->*draw_)();
}

void PauseScene::AppearUpdate(Input& input)
{
}

void PauseScene::NormalUpdate(Input& input)
{

}

void PauseScene::DisappearUpdate(Input& input)
{

}

void PauseScene::YesNoDialogUpdate(Input& input)
{

}

void PauseScene::IntervalDraw()
{

}

void PauseScene::NormalDraw()
{

}

void PauseScene::DrawMenu()
{

}
