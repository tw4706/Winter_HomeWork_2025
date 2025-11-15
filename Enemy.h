#pragma once
#include"GameObject.h"
#include<memory>
class Player;
class Enemy:public GameObject
{
protected:
	bool isTurn_;					//右を向いているかどうか
	std::shared_ptr<Player>pPlayer_;//プレイヤーのスマートポインタ
	Rect colRect_;					//当たり判定の矩形
	bool isDead_;					//死亡フラグ

public:
	Enemy(Vector2 pos,Vector2 vel);
	virtual~Enemy();

	virtual void Init() = 0;
	virtual void Update()=0;
	virtual void Draw() = 0;

	virtual void Attack() = 0;//攻撃
	virtual void Move() = 0;  //移動
	//プレイヤーのポインタをセットする関数(セッター関数)
	void SetPlayer(std::shared_ptr<Player> player) { pPlayer_ = player; }

	//当たり判定の矩形を取得する関数
	const Rect& GetColRect() const { return colRect_; }
	//死亡フラグを取得する関数
	bool IsDead() const { return isDead_; }

	//弾が当たった時の処理
	virtual void OnHit() { isDead_ = true; colRect_.SetCenter(-9999, -9999, 0, 0); }//当たり判定を消す

public:
	//地面の位置
	static constexpr float kGround = 500.0f;
};

