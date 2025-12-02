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
	//弾の発射処理
	void Shot(Input& input, BulletManager& bm);

	//ダメージを受けた時の処理
	void OnDamage();
	//リスポーン処理
	void ReSpawn();

private:
	std::vector<int>graphHandles_;//画像ハンドルの配列
	Vector2 initializePos_;//リスポーンしたときの初期位置保存用
	bool isJumping_;//ジャンプしているかどうか
	bool isDoubleJumping_;//ダブルジャンプ可能かどうか
	bool isDamaged_;//ダメージを受けているかどうか
	bool isTouching_;
	int damageTimer_;//ダメージを受けてからのタイマー
	int shotTimer_;//弾の発射間隔タイマー
	//プレイヤーの状態
	PlayerState state_;
	PlayerBulletType currentBulletType_;//現在の弾の種類
};

