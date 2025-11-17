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

	void Init()override;
	void Update()override;
	void Update(Input& input, BulletManager& bm);
	void Draw()override;

	void Move(Input& input);
	void Jump(Input& input);
	//ダメージを受けた時の処理
	void OnDamage();

	//アニメーション関連
	//アニメーションの状態セットする関数
	void SetAnimationState(PlayerState state,std::shared_ptr<Animation>anim);
	//アニメーションの更新
	void UpdateAnimation();

private:
	int idleH_;//プレイヤーの画像ハンドル
	int attackH_;//プレイヤーの画像ハンドル
	bool isJumping_;//ジャンプしているかどうか
	bool canDoubleJumping_;//ダブルジャンプ可能かどうか
	bool isDamaged_;//ダメージを受けているかどうか
	int damageTimer_;//ダメージを受けてからのタイマー
	//プレイヤーの状態
	PlayerState state_;

	//各アニメーションのスマートポインタ
	std::shared_ptr<Animation>idleAnim_;
	std::shared_ptr<Animation>attackAnim_;
	//状態とアニメーションのマップ
	std::map<PlayerState, std::shared_ptr<Animation>>animMap_;
	std::map<PlayerState, Animation::AnimationSetting>playerSetting_;
};

