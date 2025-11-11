#pragma once
#include "Enemy.h"
class Dog :public Enemy
{
public:
	Dog(Vector2 pos,Vector2 vel);
	~Dog();

	void Init()override;
	void Update()override;
	void Draw()override;

	void Attack()override;
	void Move();//移動処理

	//当たり判定取得
	const Rect& GetColRect() const override { return colRect_; }

	//弾が当たった時の処理
	void IsHit()override;

	//死亡判定取得
	bool IsDead()const override { return isDead_; }


private:
	int dogH_;//ドッグの画像ハンドル
	float timer_;//タイマー
};

