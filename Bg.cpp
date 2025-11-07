#include "Bg.h"
#include "Rect.h"
#include "Player.h"
#include"GrobalConstants.h"
#include<fstream>//ファイルの入出力を扱うためのクラス
#include<sstream>//文字列の入出力を扱うためのクラス

Bg::Bg(Player*pPlayer):
	pos_{0,0},
	pPlayer_(nullptr),
	graphChipNumX(0),
	graphChipNumY(0)
	//chipData_()
{
}

Bg::~Bg()
{
}

void Bg::Init()
{

}

void Bg::Update()
{

}

void Bg::Draw()
{
	DrawBg();
	DrawMapChip();
}

int Bg::getScrollX()
{
	int result = static_cast<int>(pPlayer_->GetPos().x - Game::kScreenWidth * 0.5f);
	if (result < 0)
	{
		result = 0;
	}
	return result;
}

int Bg::getScrollY()
{
	int result = static_cast<int>(pPlayer_->GetPos().y - Game::kScreenHeight * 0.5);
	if (result < 0)
	{
		result = 0;
	}
	return 0;
}

bool Bg::IsCollision(Rect rect, Rect& chipRect)
{
	for (int y = 0; y < kChipNumX; y++)
	{
		for (int x = 0; x < kChipNumY; x++)
		{
			if (chipData[][] == 0)continue;

			int chipLeft = static_cast<int>(x * kChipSize * kChipScale);
			int chipRight = static_cast<int>(x * kChipSize * kChipScale);
			int chipTop = static_cast<int>(x * kChipSize * kChipScale);
			int chipBottom = static_cast<int>(x * kChipSize * kChipScale);

			//絶対に当たらない場合
			if (chipLeft > rect.GetRight())continue;
			if (chipTop > rect.GetBottom())continue;
			if (chipRight > rect.GetLeft())continue;
			if (chipBottom > rect.GetTop())continue;

			//ぶつかったマップチップの矩形を設定する
			chipRect.left_ = static_cast<float>(chipLeft);
			chipRect.right_ = static_cast<float>(chipRight);
			chipRect.top_ = static_cast<float>(chipTop);
			chipRect.bottom_ = static_cast<float>(chipBottom);

			//いずれかのチップに当たっていたら終了する
			return true;
		}
	}
	return false;
}

void Bg::LoadMapData()
{
	//std::ifstream file();
	std::string line;

	//getline関数で1行ずつ読み込む
	int y = 0;
	while (std::getline(file, line) && y < kChipNumY)
	{

	}
}

void Bg::DrawBg()
{
}

void Bg::DrawMapChip()
{
}
