#pragma once
#include"GameObject.h"
#include"Enemy.h"
#include"Animation.h"
#include<vector>
#include<memory>

enum class PlayerState
{
	IDLE,
	ATTACK
};

class Input;
class BulletManager;
class Player :public GameObject
{
public:
	Player(Vector2 pos, Vector2 vel);
	~Player()override;

	virtual void Init()override;
	virtual void Update()override;
	void Update(Input& input,BulletManager&bm );
	virtual void Draw()override;

	void Move(Input& input);
	void Jump(Input& input);
	Vector2 GetPos()const;//プレイヤーの位置を取得する
	//ダメージを受けた時の処理
	void OnDamage();


private:
	int idleH_;//プレイヤーの画像ハンドル
	int attackH_;//プレイヤーの画像ハンドル
	bool isJumping_;//ジャンプしているかどうか
	bool canDoubleJumping_;//ダブルジャンプ可能かどうか
	bool isDamaged_;//ダメージを受けているかどうか
	int damageTimer_;//ダメージを受けてからのタイマー
	PlayerState state_;//プレイヤーの状態
	Animation idleAnim_;//待機アニメーション
	Animation attackAnim_;//Attackアニメーション
	Animation* currentAnim_;//現在のアニメーション
};

