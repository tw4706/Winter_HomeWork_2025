#pragma once
#include"Geometry.h"
#include<memory>
class Rect;
class Player;
class Camera;
class Bg
{
public:
	Bg(std::shared_ptr<Player> player);
	~Bg();

	void Init();

	void Draw(std::shared_ptr<Camera> pCamera);

	/// <summary>
	/// 指定した矩形と当たっているかを判定する
	/// </summary>
	/// <param name="rect">指定した矩形</param>
	/// <param name="chiprect">マップチップの矩形</param>
	/// <returns>当たっているか</returns>
	bool IsCollision(Rect& rect, Rect& chipRect);

private:

	/// <summary>
	/// マップを読み込む
	/// </summary>
	void LoadMapData();

	void DrawBg();

	/// <summary>
	/// マップチップ表示
	/// </summary>
	void DrawMapChip(std::shared_ptr<Camera>pCamera);

	int bgHandle_;//背景の画像ハンドル
	int mapHandle_;//マップチップのハンドル
	int mapData;//マップデータのハンドル
	// 画像に含まれるマップチップの数
	int graphChipNumX_;
	int graphChipNumY_;

	int mapChipData_[200][40];//マップデータ

	// マップデータのサイズ
	struct Size
	{
		int width;
		int height;
	};

	Vector2 pos_;//座標
	std::shared_ptr<Player>pPlayer_;
};

