#pragma once
#include"GameObject.h"
#include"Animation.h"
#include<memory>
#include<vector>
#include<string>

class Player;
class Enemy:public GameObject
{
public:
	//地面の位置
	static constexpr float kGround = 500.0f;
protected:
	bool isTurn_;					//右を向いているかどうか
	std::shared_ptr<Player>pPlayer_;//プレイヤーのスマートポインタ
	Rect colRect_;					//当たり判定の矩形
	bool isDead_;					//死亡フラグ
	int hp_;						//体力
	int currentState_;
	std::vector<std::shared_ptr<Animation>> animations_;//アニメーション配列

public:
	Enemy(Vector2 pos,Vector2 vel);
	virtual~Enemy();

	virtual void Init() = 0;
	virtual void Update()=0;
	virtual void Draw() = 0;

	virtual void Move() = 0;//移動
	//プレイヤーのポインタをセットする関数(セッター関数)
	void SetPlayer(std::shared_ptr<Player> player) { pPlayer_ = player; }

	//当たり判定の矩形を取得する関数
	const Rect& GetColRect() const { return colRect_; }
	//死亡フラグを取得する関数
	bool IsDead() const { return isDead_; }

	virtual bool IsBoss() const { return false; }

	//弾が当たった時の処理
	//当たり判定を消す
	virtual void OnHit(int damage);
};

