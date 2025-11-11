#pragma once
#include"GameObject.h"
#include"Geometry.h"
#include<memory>
class Input;
class Bullet:public GameObject
{
public:
	Bullet(Vector2 pos,Vector2 vel);
	~Bullet()override;

	void Init()override;
	void Update()override;
	void Update(Input&input);
	void Draw()override;

	/// <summary>
	/// 当たり判定を取得
	/// </summary>
	/// <returns>当たり判定情報</returns>
	Rect GetColRect()const { return colRect_; }

	// 弾が消えたかどうかを返す
	bool IsDead()const { return isDead_; }

private:
	Vector2 pos_;//位置
	Vector2 vel_;//速度
	int bulletH_;//弾の画像ハンドル
};

