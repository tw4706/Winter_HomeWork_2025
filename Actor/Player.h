#pragma once
#include"GameObject.h"
#include"Enemy.h"
#include"Animation.h"
#include"Bullet.h"
#include<vector>
#include<memory>

enum class PlayerState
{
	Idle,
	Attack
};

class Input;
class BulletManager;
class Player :public GameObject
{
public:
	Player(Vector2 pos, Vector2 vel);
	~Player()override;

	void Init()override;
	void Update()override;
	void Update(Input& input, BulletManager& bm);
	void Draw()override;

	void Move(Input& input);
	void Jump(Input& input);
	//ダメージを受けた時の処理
	void OnDamage();

private:
	std::vector<int>graphHandles_;
	bool isJumping_;//ジャンプしているかどうか
	bool isDoubleJumping_;//ダブルジャンプ可能かどうか
	bool isDamaged_;//ダメージを受けているかどうか
	int damageTimer_;//ダメージを受けてからのタイマー
	int shotTimer_;//弾の発射間隔タイマー
	//プレイヤーの状態
	PlayerState state_;
	PlayerBulletType currentBulletType_;//現在の弾の種類
};

