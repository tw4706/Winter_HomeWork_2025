#include "Bg.h"
#include "Player.h"
#include "Camera.h"
#include"GlobalConstants.h"
#include<DxLib.h>
#include <fstream>
#include <sstream>

namespace
{
	constexpr float kMapWidth = 3000.0f;//マップ全体の幅
	constexpr float kMapHeight = 1080.0f;//マップ全体の高さ

	constexpr int kChipSize = 16;//マップチップ1つの大きさ

	constexpr int kChipNumX = 50;
	constexpr int kChipNumY = 20;
}

Bg::Bg(std::shared_ptr<Player> player):
	pos_{},
	pPlayer_(player)
{
	pPlayer_ = player;
	mapHandle_ = LoadGraph("data/Map/mapChip.png");
	int graphWidth = 0;
	int graphHeight = 0;
	GetGraphSize(mapHandle_, &graphWidth, &graphHeight);


	graphChipNumX_ = (graphWidth > 0) ? graphWidth / kChipSize : 1;
	graphChipNumY_ = (graphHeight > 0) ? graphHeight / kChipSize : 1;

	//マップデータの読み込み
	LoadMapData();
}

Bg::~Bg()
{
	DeleteGraph(mapHandle_);
}

void Bg::Init()
{

}

void Bg::Draw(std::shared_ptr<Camera> pCamera)
{
	DrawBg();
	DrawMapChip(pCamera);
}

bool Bg::IsCollision(Rect& rect, Rect& chipRect)
{
	for (int y = 0; y < kChipNumY; y++)
	{
		for (int x = 0; x < kChipNumX; x++)
		{
			//当たり判定のあるマップチップとのみ判定を行う
			if (mapChipData_[x][y] == 0)continue;

			int chipLeft = static_cast<int>(x * kChipSize);
			int chipRight = static_cast<int>(chipLeft + kChipSize);
			int chipTop = static_cast<int>(y * kChipSize);
			int chipBottom = static_cast<int>(chipTop + kChipSize);

			//絶対に当たらないパターン
			if (chipLeft > rect.GetRight()) continue;
			if (chipRight < rect.GetLeft()) continue;
			if (chipTop > rect.GetBottom()) continue;
			if (chipBottom < rect.GetTop()) continue;

			//当たっている
			chipRect.left_ = static_cast<float>(chipLeft);
			chipRect.right_ = static_cast<float>(chipRight);
			chipRect.top_ = static_cast<float>(chipTop);
			chipRect.bottom_ = static_cast<float>(chipBottom);

			//あっている中からどれかしら当たっていたら終了する
			return true;
		}
	}
	return false;
}


void Bg::LoadMapData()
{
	// まずサイズを確保
	mapChipData_.resize(kChipNumX);
	for (int x = 0; x < kChipNumX; ++x) {
		mapChipData_[x].resize(kChipNumY, 0);
	}

	std::ifstream file("data/Map/Stage1.csv");
	if (!file.is_open()) {
		printf("CSVファイルが開けませんでした\n");
		return;
	}

	std::string line;
	int y = 0;

	while (std::getline(file, line) && y < kChipNumY)
	{
		if (line.find_first_not_of("0123456789,") != std::string::npos) {
			continue; // 不正な行はスキップ
		}

		std::stringstream stream(line);
		std::string field;
		int x = 0;

		while (std::getline(stream, field, ',') && x < kChipNumX)
		{
			try {
				mapChipData_[x][y] = std::stoi(field);
			}
			catch (...) {
				mapChipData_[x][y] = 0; // 変換失敗時は0
			}
			x++;
		}
		y++;
	}
}

void Bg::DrawBg()
{
	Size bgSize = { 0,0 };
	GetGraphSize(mapHandle_, &bgSize.width, &bgSize.height);
}

void Bg::DrawMapChip(std::shared_ptr<Camera>pCamera)
{
	float scale = 2.0f; // 16px → 32px（見やすくする）
	float camLeft = -pCamera->GetOffset().x;
	float camRight = camLeft + Game::kScreenWidth;
	float camTop = -pCamera->GetOffset().y;
	float camBottom = camTop + Game::kScreenHeight;

	for (int y = 0; y < kChipNumY; y++)
	{
		for (int x = 0; x < kChipNumX; x++)
		{
			float chipLeft = x * kChipSize * scale;
			float chipRight = chipLeft + kChipSize * scale;
			float chipTop = y * kChipSize * scale;
			float chipBottom = chipTop + kChipSize * scale;

			if (chipRight < camLeft || chipLeft > camRight ||
				chipBottom < camTop || chipTop > camBottom)
			{
				continue;
			}

			int posX = static_cast<int>(chipLeft + pCamera->GetOffset().x);
			int posY = static_cast<int>(chipTop + pCamera->GetOffset().y);

			int chipNum = mapChipData_[x][y];
			int srcX = kChipSize * (chipNum % graphChipNumX_);
			int srcY = kChipSize * (chipNum / graphChipNumX_);

			DrawRectRotaGraph(
				posX + (kChipSize * scale) / 2,
				posY + (kChipSize * scale) / 2,
				srcX, srcY,
				kChipSize, kChipSize,
				scale, 0.0f,
				mapHandle_, true);
#ifdef _DEBUG
			//当たり判定
			DrawBoxAA(
				static_cast<int>(x * kChipSize - pCamera->GetOffset().x),
				static_cast<int>(y * kChipSize - pCamera->GetOffset().y),
				static_cast<int>(x * kChipSize + kChipSize - pCamera->GetOffset().x),
				static_cast<int>(y * kChipSize + kChipSize - pCamera->GetOffset().y),
				GetColor(255, 255, 0), false);
#endif
		}
	}
}
