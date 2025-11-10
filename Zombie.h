#pragma once
#include "Enemy.h"
#include<memory>
class Player;
class Zombie :public Enemy
{
public:
	Zombie(Vector2 pos,Vector2 vel);
	virtual ~Zombie();

	void Init()override;
	void Update()override;
	void Draw()override;

	void Attack()override;	//攻撃処理
	void Move();			//移動処理

	//当たり判定取得
	const Rect& GetRect() const override { return colRect_; }
	//死亡判定取得
	bool IsDead()const override { return isDead_; }

private:
	int zombieH_;			//ゾンビの画像ハンドル
	Rect colRect_;			//当たり判定用矩形
	bool isDead_;			//死亡フラグ
};

