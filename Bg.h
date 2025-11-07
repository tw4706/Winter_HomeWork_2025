#pragma once

#include"Geometry.h"

class Player;
class Rect;
class Bg
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Bg(Player* pPlayer);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Bg();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 横のスクロール量を決定する
	/// </summary>
	/// <returns></returns>
	int getScrollX();

	/// <summary>
	/// 縦のスクロール量を決定する
	/// </summary>
	/// <returns></returns>
	int getScrollY();

	/// <summary>
	/// 指定した矩形と当たっているか
	/// </summary>
	/// <param name="rect">指定した矩形</param>
	/// <param name="chipRect">マップチップの矩形</param>
	/// <returns>当たっているか</returns>
	bool IsCollision(Rect rect, Rect& chipRect);
private:

	/// <summary>
	/// マップを読み込む
	/// </summary>
	void LoadMapData();

	/// <summary>
	/// 背景表示
	/// </summary>
	void DrawBg();

	/// <summary>
	/// マップチップ表示
	/// </summary>
	void DrawMapChip();

private:
	int bgHandle_;//背景画像
	int mapHandle_;//マップチップ画像
	int mapData_;//マップデータ
	Vector2 pos_;//背景位置
	Player* pPlayer_;

	//画像に含まれるマップチップの数
	int graphChipNumX;
	int graphChipNumY;

	//int chipData[][];

	struct Size
	{
		int width;//幅
		int height;//高さ
	};
};

