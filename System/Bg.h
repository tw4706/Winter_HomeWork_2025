#pragma once
#include"Geometry.h"
#include"StageType.h"
#include<memory>
#include<vector>

class Rect;
class Player;
class Camera;
class Bg
{
public:
	Bg(StageType stageType);
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
	void LoadMapData(StageType stageType);

	void UpdateMidBg(std::shared_ptr<Camera> pCamera);

	void DrawBg(std::shared_ptr<Camera> pCamera);

	void DrawMidBg(std::shared_ptr<Camera> pCamera);

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

	int mapChipData_[40][200];//マップデータ

	// マップデータのサイズ
	struct Size
	{
		int width;
		int height;
	};

	Vector2 pos_;//座標
	std::shared_ptr<Player>pPlayer_;
	float prevCameraX_;		//前フレームのカメラのX座標
	int midbgHandle_;		//中景画像ハンドル
	float midPosY_;			//画面上での縦位置
	float midScrollRate_;	//パララックス倍率
	float midBgScrollX_;	//中景のスクロールX座標
	StageType stageType_;	//ステージの種類
};

