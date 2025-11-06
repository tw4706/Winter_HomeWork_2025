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

};

