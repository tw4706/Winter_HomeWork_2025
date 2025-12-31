#include "EffectManager.h"


void EffectManager::Update()
{
	//エフェクトの更新
	for (auto& effect : effects_)
	{
		effect->Update();
	}
	//消えたエフェクトを削除
	effects_.erase(
		std::remove_if(
			effects_.begin(),
			effects_.end(),
			[](const std::shared_ptr<Effect>& effect)
			{return effect->IsDead();}),effects_.end());
}

void EffectManager::Draw()
{
	//エフェクトの描画
	for (auto& effect : effects_)
	{
		effect->Draw();
	}
}

void EffectManager::AddEffect(std::shared_ptr<Effect> effect)
{
	effect->SetCameraOffset(cameraOffset_);
	effects_.push_back(effect);
}

void EffectManager::SetCameraOffset(const Vector2& offset)
{
	cameraOffset_ = offset;
	for(auto& effect : effects_)
	{
		effect->SetCameraOffset(offset);
	}
}
