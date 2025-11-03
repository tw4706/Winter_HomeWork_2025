#pragma once
#include "Enemy.h"
#include<memory>
class Player;
class Zombie :public Enemy
{
public:
	Zombie(Vector2 pos);
	virtual ~Zombie();

	void Init()override;
	void Update()override;
	void Draw()override;

	void Attack()override;	//攻撃処理
	void Move()override;	//移動処理
	//プレイヤーのポインタをセットする関数(セッター関数)
	void SetPlayer(Player* player);

private:
	int zombieH_;	//ゾンビの画像ハンドル
	Player* pPlayer_ = nullptr; // プレイヤーのポインタ
};

