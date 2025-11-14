#pragma once
#include"Geometry.h"
#include<memory>
class Player;
class Bg
{
public:
	Bg(std::shared_ptr<Player> player);
	~Bg();

	void Init();

	void Draw();

	/// <summary>
	/// マップの描画
	/// </summary>
	void DrawBg();

	/// <summary>
	/// 横のスクロール量を決定する
	/// </summary>
	/// <returns>横スクロール量</returns>
	int GetScrollX();

	/// <summary>
	/// 縦のスクロール量を決定する
	/// </summary>
	/// <returns>縦のスクロール量</returns>
	int GetScrollY();

private:

	int handle_;//画像ハンドル
	Vector2 pos_;//座標
	std::shared_ptr<Player>pPlayer_;
};

