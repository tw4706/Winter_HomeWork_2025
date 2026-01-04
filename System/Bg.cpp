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
	constexpr float kScale = 3.0f;

	constexpr int kChipNumX = 200;
	constexpr int kChipNumY = 40;
}

Bg::Bg(StageType stageType):
	pos_{0,0},
	prevCameraX_(0.0f),
	midbgHandle_(-1),
	midPosY_(0.0f),
	midScrollRate_(0.5f),
	midBgScrollX_(0.0f),
	stageType_(stageType)
{
	mapHandle_ = LoadGraph("data/Map/mapChip.png");
	bgHandle_ = LoadGraph("data/map/bg.png");
	midbgHandle_ = LoadGraph("data/map/middle1.png");

	int graphWidth = 0;
	int graphHeight = 0;
	GetGraphSize(mapHandle_, &graphWidth, &graphHeight);

	graphChipNumX_ = (graphWidth > 0) ? graphWidth / kChipSize : 1;
	graphChipNumY_ = (graphHeight > 0) ? graphHeight / kChipSize : 1;


	//マップデータの読み込み
	LoadMapData(stageType_);
}

Bg::~Bg()
{
	DeleteGraph(mapHandle_);
	DeleteGraph(bgHandle_);
	DeleteGraph(midbgHandle_);
}

void Bg::Init()
{

}

void Bg::Draw(std::shared_ptr<Camera> pCamera)
{

	DrawBg(pCamera);

	UpdateMidBg(pCamera);
	DrawMidBg(pCamera);

	DrawMapChip(pCamera);
}

bool Bg::IsCollision(Rect& rect, Rect& chipRect)
{
	for (int y = 0; y < kChipNumY; y++)
	{
		for (int x = 0; x < kChipNumX; x++)
		{
			//当たり判定のあるマップチップとのみ判定を行う
			if (mapChipData_[y][x] == 0)continue;

			int chipLeft = static_cast<int>(x * kChipSize*kScale);
			int chipRight = static_cast<int>(chipLeft + kChipSize * kScale);
			int chipTop = static_cast<int>(y * kChipSize * kScale);
			int chipBottom = static_cast<int>(chipTop + kChipSize * kScale);

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

void Bg::LoadMapData(StageType stageType)
{
	//配列を初期化
	for (int x = 0; x < kChipNumX; ++x)
	{
		for (int y = 0; y < kChipNumY; ++y)
		{
			mapChipData_[y][x] = 0;
		}
	}
	std::string fileName;

	//ステージに応じたマップデータを設定
	switch (stageType)
	{
	case StageType::Tutorial:
		fileName = "data/Map/Tutorial.csv"; break;
	case StageType::Stage1:
		fileName = "data/Map/Stage1.csv"; break;
	case StageType::Stage2: 
		fileName = "data/Map/Stage2.csv"; break;
	default: 
		fileName = "data/Map/Stage1.csv"; break;
	}

	std::ifstream file(fileName);
	std::string line;

	//1行ずつ読み込み
	int y = 0;
	while (std::getline(file, line) && y < kChipNumY)
	{
		if (line.find_first_not_of("0123456789,") != std::string::npos) {
			continue;
		}

		std::stringstream stream(line);
		std::string field;
		int x = 0;
		while (std::getline(stream, field, ',') && x < kChipNumX)
		{
			// 文字列をint型に変換してmapChipDataに追加する
			mapChipData_[y][x] = std::stoi(field);
			x++;
		}
		y++;
	}
}

void Bg::UpdateMidBg(std::shared_ptr<Camera> pCamera)
{
	// 中景のX座標をカメラに合わせてスクロール
	float cameraMoveX = pCamera->GetOffset().x - prevCameraX_;
	midBgScrollX_ += cameraMoveX * midScrollRate_;
	prevCameraX_ = pCamera->GetOffset().x;
}

void Bg::DrawBg(std::shared_ptr<Camera> pCamera)
{
	DrawGraph(0, 0, bgHandle_, false);
}

void Bg::DrawMidBg(std::shared_ptr<Camera> pCamera)
{
	if (midbgHandle_ < 0) return;

	// 横にループさせる場合
	int graphWidth = 0, graphHeight = 0;
	GetGraphSize(midbgHandle_, &graphWidth, &graphHeight);

	// 縦位置を固定
	int drawX = static_cast<int>(midBgScrollX_);
	midPosY_ = Game::kScreenHeight / 2 - graphHeight / 2; // 好みのY位置に固定

	for (int x = drawX % graphWidth - graphWidth; x < Game::kScreenWidth; x += graphWidth)
	{
		DrawGraph(x, midPosY_, midbgHandle_, true);
	}
}


void Bg::DrawMapChip(std::shared_ptr<Camera>pCamera)
{
	for(int y = 0; y < kChipNumY; y++)
	{
		for (int x = 0; x < kChipNumX; x++)
		{
			int tileW = (int)(kChipSize * kScale);
			int tileH = (int)(kChipSize * kScale);

			int posX = (int)(x * kChipSize * kScale + pCamera->GetOffset().x);
			int posY = (int)(y * kChipSize * kScale + pCamera->GetOffset().y);

			//画面外のものは描画しない
			if (posX + tileW < 0) continue;
			if (posX > Game::kScreenWidth) continue;
			if (posY + tileH < 0) continue;
			if (posY > Game::kScreenHeight) continue;

			//マップチップ番号を取得
			int chipNum = mapChipData_[y][x];

			//マップチップの切り出し座標
			int srcX = kChipSize * (chipNum % graphChipNumX_);
			int srcY = kChipSize * (chipNum / graphChipNumX_);

			//マップチップの描画
			DrawRectRotaGraph(
				posX + static_cast<int>(kChipSize * kScale * 0.5f),
				posY + static_cast<int>(kChipSize * kScale * 0.5f),
				srcX, srcY,
				kChipSize, kChipSize,
				kScale, 0.0f,
				mapHandle_, true);
#ifdef _DEBUG
			//当たり判定のあるチップだけ矩形を描画
			if (chipNum != 0)
			{
				int left = posX;
				int top = posY;
				int right = posX + static_cast<int>(kChipSize * kScale);
				int bottom = posY + static_cast<int>(kChipSize * kScale);

				DrawBox(left, top, right, bottom, GetColor(255, 0, 0), false);
			}
#endif
		}
	}
}
