#pragma once
#include"GameObject.h"
#include"Animation.h"
#include<memory>
#include<string>

enum class EnemyState
{
	Idle,
	Walk,
	Jump,
	Attack,
	Damage,
	Death
};

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
	int idleH_;
	int jumpH_;
	int attackH_;
	std::map<EnemyState, std::shared_ptr<Animation>>animMap_;
	static std::map<std::string,
		std::map<EnemyState, Animation::AnimationSetting>>enemySettings_;

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
	//当たり判定を消す
	virtual void OnHit() { isDead_ = true; colRect_.SetCenter(-9999, -9999, 0, 0); }

protected:
	//アニメーションの初期化
	void InitAnimation(const std::string&enemyType,int handle,int frameW,int frameH);

	//アニメーションの状態をセットする関数
	void SetAnimationState(EnemyState state, std::shared_ptr<Animation>anim);

	//アニメーションの更新
	void UpdateAnimation();
};

