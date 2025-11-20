#include "Bg.h"
#include "Player.h"
#include"GlobalConstants.h"
#include<DxLib.h>

namespace
{
	constexpr float kMapWidth = 5000.0f;//マップ全体の幅
	constexpr float kMapHeight = 1080.0f;//マップ全体の高さ
}

Bg::Bg(std::shared_ptr<Player> player):
	handle_(-1),
	pos_{},
	pPlayer_(player)
{
	pPlayer_ = player;
}

Bg::~Bg()
{
	DeleteGraph(handle_);
}

void Bg::Init()
{
	handle_ = LoadGraph("data/map/bg.png");
}

void Bg::Draw()
{
	DrawBg();
}

void Bg::DrawBg()
{
	//画面サイズを取得
	Size bgSize = { 0,0 };
	GetGraphSize(handle_, &bgSize.w, &bgSize.h);

	int scrollBg = GetScrollX() % bgSize.w;

	//DrawGraph(-scrollBg, pos_.y, handle_, true);
	if (scrollBg > 0)
	{
		DrawGraph(bgSize.w - scrollBg, Game::kScreenHeight - bgSize.h, handle_, true);
	}
	
}

int Bg::GetScrollX()
{
	float playerX = pPlayer_->GetPos().x;
	//画面中央にプレイヤーが来るスクロール量を計算
	float result = playerX-(Game::kScreenWidth * 0.5);

	if (result < 0)
	{
		result = 0;
	}
	if (result > kMapWidth - Game::kScreenWidth)
	{
		result = kMapWidth - Game::kScreenWidth;
	}
	return result;
}

int Bg::GetScrollY()
{
	int result = static_cast<int>(pPlayer_->GetPos().y - Game::kScreenHeight * 0.5);

	if (result < 0)
	{
		result = 0;
	}
	if (result > kMapHeight - Game::kScreenHeight)
	{
		result = kMapHeight - Game::kScreenHeight;
	}
	return result;
}
