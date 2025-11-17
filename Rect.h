#pragma once
#include"Geometry.h"
class Rect
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Rect();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual~Rect();

	/// <summary>
	/// 矩形の描画
	/// </summary>
	/// <param name="color">矩形の色</param>
	/// <param name="isFill">太さ</param>
	void Draw(unsigned int color,bool isFill);

	/// <summary>
	/// 左上座標と幅高さを指定
	/// </summary>
	/// <param name="left">左</param>
	/// <param name="top">上</param>
	/// <param name="width">幅</param>
	/// <param name="height">高さ</param>
	void SetLT(float left, float top, float width, float height);

	/// <summary>
	/// 中心座標と幅高さの指定
	/// </summary>
	/// <param name="x">中心のx座標</param>
	/// <param name="y">中心のy座標</param>
	/// <param name="width">幅</param>
	/// <param name="height">高さ</param>
	void SetCenter(float x, float y, float width, float height);

	/// <summary>
	/// 矩形の幅を取得
	/// </summary>
	/// <returns>矩形の幅</returns>
	float GetWidth()const;

	/// <summary>
	/// 矩形の高さを取得
	/// </summary>
	/// <returns>矩形の高さ</returns>
	float GetHeight()const;


	/// <summary>
	/// 矩形の中心座標を取得
	/// </summary>
	/// <returns>矩形の中心座標</returns>
	Vector2 GetCenter()const;

	/// <summary>
	/// 矩形の左上のX座標を取得
	/// </summary>
	/// <returns>左上のX座標</returns>
	float GetLeft()const { return left_; }

	/// <summary>
	/// 矩形の左上の座標を取得
	/// </summary>
	/// <returns>左上のX座標を取得</returns>
	float GetTop()const { return top_; }

	/// <summary>
	/// 矩形の右上のX座標を取得
	/// </summary>
	/// <returns>右下のX座標</returns>
	float GetRight()const { return right_; }

	/// <summary>
	/// 矩形の右下のX座標を取得
	/// </summary>
	/// <returns>右下のY座標</returns>
	float GetBottom()const { return bottom_; }

	/// <summary>
	/// 矩形の当たり判定
	/// </summary>
	/// <param name="rect">矩形情報</param>
	/// <returns>当たっているかどうか</returns>
	bool IsCollision(const Rect& rect)const;

	/// <summary>
	/// スクロール情報を適用した矩形の描画
	/// </summary>
	/// <param name="scrollX">横スクロール量</param>
	/// <param name="color">矩形の色</param>
	/// <param name="isFill">太さ</param>
	void DrawScroll(int scrollX, int scrollY, unsigned int color, bool isFill);


public:
	float left_;	//左上のX座標
	float top_;	//左上のY座標
	float right_;	//右下のX座標
	float bottom_;	//右上のY座標
};

