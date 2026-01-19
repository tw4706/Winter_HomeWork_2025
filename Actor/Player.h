#pragma once
#include"GameObject.h"
#include"Enemy.h"
#include"Animation.h"
#include"Bullet.h"
#include"StageType.h"
#include<vector>
#include<memory>
#include<functional>

//プレイヤーの状態
enum class PlayerState
{
	Idle,
	Attack,
	Walk,
	Jump,
	Hurt,
	Death
};

//プレイヤーの操作状態
enum class PlayerControl
{
	Normal,
	AutoWalking,
	Stop,
	TitleDemo
};

//チュートリアルで行ってほしいアクション
enum class TutorialAction
{
	Move,
	Jump,
	DoubleJump,
	Attack,
	WeaponChange
};

class Input;
class GameProgress;
class BulletManager;
class Player :public GameObject
{
public:
	//ゲームオーバーを通知する
	std::function<void()>onGameOver_;

	Player(Vector2 pos, Vector2 vel);
	~Player()override;

	void Init()override;
	void Update()override {};
	void Update(Input& input, BulletManager& bm, StageType stage);
	void Draw()override;

	void Move(Input& input);
	void Jump(Input& input);
	//弾の発射処理
	void Shot(Input& input, BulletManager& bm);

	//座標の設定(デバッグで使う用)
	void SetPos(const Vector2& pos) { pos_ = pos; }

	//ダメージを受けた時の処理
	void OnDamage(float enemyX);

	void Dead();
	bool IsDead()const { return !isAlive_; }
	bool IsDeadAnimFinished()const { return isDeathAnimFinished_; }

	void UpdateState(Input& input);

	//たいまつが使えるかどうか
	void UnlockTorch() { isUnlockedTorch_ = true; }
	bool IsUnlockedTorch() const { return isUnlockedTorch_; }

	//ボスを倒した後の自動移動
	void StartAutoWalk(int dir);

	//現在の弾の種類を取得
	BulletType GetCurrentBulletType() const { return currentBulletType_; }

	void SetCamera(std::shared_ptr<Camera> camera);

	//ゲームの進行状況を設定する関数
	void SetGameProgress(GameProgress* progress) { gameProgress_ = progress; }

	//チュートリアルアクションの通知を行う関数	
	void OnTutorialAction(TutorialAction action);

	//プレイヤーが操作可能かどうかの設定と取得
	void SetControllable(bool canControl);
	bool IsControllable() const;

private:
	std::vector<int>graphHandles_;//画像ハンドルの配列
	Vector2 initializePos_;//リスポーンしたときの初期位置保存用
	int hp_;//体力
	bool isJumping_;//ジャンプしているかどうか
	bool isDoubleJumping_;//ダブルジャンプ可能かどうか
	bool isDamaged_;//ダメージを受けているかどうか
	bool isTouching_;//地面に接触しているかどうか
	bool isAttacking_;//攻撃しているかどうか
	int attackTimer_;//攻撃のタイマー
	int damageTimer_;//ダメージを受けてからのタイマー
	int shotTimer_;//弾の発射間隔タイマー
	bool isAlive_;//プレイヤーが生きているかどうか
	bool isDeathAnimFinished_;//死亡アニメーションが終了したかどうか
	bool isUnlockedTorch_;//たいまつが使えるかどうか
	int autoWalkDir_;//自動移動の方向
	float autoWalkSpeed_;//自動移動の速度
	int hitStopTimer_ = 0;//ヒットストップ用タイマー
	float hitStopFactor_ = 0.0f;
	int landingTimer_ = 0;//地面着地用タイマー

	StageType currentStage_;//現在のステージの種類
	PlayerControl controlMode_;//プレイヤーの操作している状態
	PlayerState state_;	//プレイヤーの状態
	PlayerState prevState_;//前のフレームのプレイヤーの状態
	BulletType currentBulletType_;//現在の弾の種類
	std::shared_ptr<Camera> pCamera_;
	std::vector<std::shared_ptr<Animation>>animations_;//アニメーションの配列
	GameProgress* gameProgress_;//ゲームの進行状況を管理するクラスへのポインタ
};

