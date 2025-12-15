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

Bg::Bg():
	pos_{0,0},
	prevCameraX_(0.0f),
	midbgHandle_(-1),
	midPosY_(0.0f),
	midScrollRate_(0.5f),
	midBgScrollX_(0.0f)
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
	LoadMapData();
}

Bg::~Bg()
{
	DeleteGraph(mapHandle_);
	DeleteGraph(bgHandle_);
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

void Bg::LoadMapData()
{
	//配列を初期化
	for (int x = 0; x < kChipNumX; ++x)
	{
		for (int y = 0; y < kChipNumY; ++y)
		{
			mapChipData_[y][x] = 0;
		}
	}

	std::ifstream file("data/Map/Stage1.csv");
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
	// カメラのX移動量
	float cameraX = pCamera->GetOffset().x;

	// 中景のスクロール量を更新
	midBgScrollX_ += (cameraX - prevCameraX_) * midScrollRate_;

	// 中景画像の幅を取得
	int midBgWidth, midBgHeight;
	GetGraphSize(midbgHandle_, &midBgWidth, &midBgHeight);

	// ループ処理
	if (midBgScrollX_ > midBgWidth) midBgScrollX_ -= midBgWidth;
	if (midBgScrollX_ < 0) midBgScrollX_ += midBgWidth;

	// 前回カメラ位置を保存
	prevCameraX_ = cameraX;
}

void Bg::DrawBg(std::shared_ptr<Camera> pCamera)
{
	DrawGraph(0, 0, bgHandle_, false);
}

void Bg::DrawMidBg(std::shared_ptr<Camera> pCamera)
{
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
