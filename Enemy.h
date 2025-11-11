#pragma once
#include"GameObject.h"
#include"Player.h"
class Enemy:public GameObject
{
protected:
	bool isRight_;			//右を向いているかどうか
	Player* pPlayer_;		//プレイヤーのポインタ

public:
	Enemy(Vector2 pos,Vector2 vel);
	virtual~Enemy();

	virtual void Init() = 0;
	virtual void Update()=0;
	virtual void Draw() = 0;

	virtual void Attack() = 0;//攻撃
	virtual void Move() = 0;  //移動	
	//位置を取得する関数(ゲッター関数)
	const Vector2& GetPos()const { return pos_; }
	//プレイヤーのポインタをセットする関数(セッター関数)
	void SetPlayer(Player* player);

	//当たり判定の矩形を取得する関数
	virtual const Rect& GetRect()const = 0;

	//弾が当たった時の処理
	virtual void IsHit() = 0;
	
	//敵の死亡処理
	virtual bool IsDead() const = 0;
};

