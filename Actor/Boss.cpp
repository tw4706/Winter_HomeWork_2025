#include "Boss.h"
#include "Player.h"
#include"Camera.h"
#include "BulletManager.h"
#include<Dxlib.h>
#include<cassert>
#include<string>

namespace
{
	enum Graph
	{
		kIdleGraph,
		kAttackGraph,
		kFlyGraph,
		kHurtGraph,
		kDeadGraph,

		kGraphNum
	};

	const std::string kGraphName[kGraphNum] =
	{
		"data/Enemy/IDLE.png",
		"data/Enemy/ATTACK.png",
		"data/Enemy/FLYING.png",
		"data/Enemy/HURT.png",
		"data/Enemy/DEATH.png"
	};

	static_assert(static_cast<int>(kGraphNum)== _countof(kGraphName));

	//ボスの描画・当たり判定関連
	constexpr int kGraphWidth = 81;
	constexpr int kGraphHeight = 71;
	constexpr int kGraphHalfW = kGraphWidth /2;
	constexpr int kGraphHalfH = kGraphHeight /2;
	constexpr float kScale = 4.0f;
	constexpr float kColScale = 2.0f;
	constexpr float kColSize = 64.0f;

	//ボスの挙動関連
	constexpr float kKnockBackPos = 2.0f;
	constexpr float kComeBackPos = 0.5f;
	constexpr float kSpeed = 0.5f;

	//近づかれすぎた時の逃げる速度
	constexpr float kFlySpeed = 0.4f;
	constexpr float kEscapeSpeed = 1.8f;
	constexpr float kBulletSpeed = 3.0f;
	constexpr float kShotInterval = 5.0f;
	constexpr int KMaxHp = 30;
	constexpr float kRushSpeed = 4.5f;   //突進する速度
	constexpr int   kRushTime = 18;     //突進するフレーム数
	constexpr float kActiveDistance = 500.0f;

	//地形・距離関連
	constexpr float kGravity = 0.5f;
	constexpr float kDistance = 400.0f;
	constexpr float kGround = 1764.0f;

	//カメラの揺れ関連
	constexpr int kCameraDuration = 10;
	constexpr int kCameraMagnitude = 8;

	//状態ごとの総フレーム数
	constexpr int kIdleFrameCount = 4;
	constexpr int kAttackFrameCount = 8;
	constexpr int kFlyFrameCount = 4;
	constexpr int kHurtFrameCount = 4;
	constexpr int kDeathFrameCount = 7;

	//状態ごとの更新フレームの間隔
	constexpr int kIdleFrameInterval = 6;
	constexpr int kAttackFrameInterval = 6;
	constexpr int kFlyFrameInterval = 6;
	constexpr int kHurtFrameInterval = 6;
	constexpr int kDeathFrameInterval = 6;

	//状態ごとのフレーム数とフレームの間隔
	const int frameCounts[kGraphNum] = { kIdleFrameCount,kAttackFrameCount,
		kFlyFrameCount, kHurtFrameCount, kDeathFrameCount };
	const int frameIntervals[kGraphNum] = { kIdleFrameInterval,
		kAttackFrameInterval, kFlyFrameInterval, kHurtFrameInterval, kDeathFrameInterval };
}

Boss::Boss(Vector2 pos, Vector2 vel, std::shared_ptr<Player>player, BulletManager*bm,std::shared_ptr<Camera>camera):
	Enemy(pos,vel),
	pPlayer_(player),
	pBm_(bm),
	pCamera_(camera),
	currentState_(BossState::Idle),
	stateTimer_(0),
	handle_(-1),
	shotTimer_(0.0f),
	shotInterval_(kShotInterval),
	hp_(KMaxHp),
	backPos_(pos),
	hasShot_(false),
	knockbackDir_(0),
	isCharging_(false),
	chargeVel_(0.0f),
	isActive_(false),
	escapeTimer_(0.0f)
{
	colSize_ = kColSize;
	SetUseGravity(false);
}

Boss::~Boss()
{
	for (auto& handle : graphHandles_)
	{
		DeleteGraph(handle);
	}
}

void Boss::Init()
{
	graphHandles_.resize(kGraphNum);
	animations_.resize(kGraphNum);

	for (int i = 0; i < kGraphNum; i++)
	{
		graphHandles_[i] = LoadGraph(kGraphName[i].c_str());
		animations_[i] = std::make_shared<Animation>(
			graphHandles_[i],
			kGraphWidth,
			kGraphHeight,
			frameCounts[i],
			frameIntervals[i],
			kScale,
			false,0);
	}
	currentState_ = BossState::Idle;
	stateTimer_ = 0;
	handle_ = graphHandles_[kIdleGraph];
}

void Boss::Update()
{
	Enemy::Update();

	float distance = std::abs(pPlayer_->GetPos().x - pos_.x);

	// プレイヤーが一定距離まで近づいたら起動
	if (!isActive_)
	{
		if (distance < kActiveDistance)
		{
			isActive_ = true;
			stateTimer_ = 0;
		}
		else
		{
			//起動前は当たり判定はなしにする
			colRect_.SetCenter(pos_.x, pos_.y, 0, 0);

			//Idleアニメーションだけ再生
			animations_[static_cast<int>(BossState::Idle)]->Update();
			return;
		}
	}

	//常にプレイヤーの方向を向く
	isTurn_ = (pPlayer_->GetPos().x < pos_.x);

	colRect_.SetCenter(pos_.x-20, pos_.y, colSize_ * kColScale, colSize_ * kColScale);

	//アニメーションの更新
	animations_[static_cast<int>(currentState_)]->Update();

	switch (currentState_)
	{
	case BossState::Idle:
		UpdateIdle();
		break;
	case BossState::Attack:
		UpdateAttack();
		break;
	case BossState::Fly:
		UpdateFly();
		break;
	case BossState::Hurt:
		UpdateHurt();
		break;
	case BossState::Dead:
		UpdateDead();
		break;
	}

#ifdef _DEBUG
	//Kキーで即死
	if (CheckHitKey(KEY_INPUT_K))
	{
		hp_ = 0;
		pCamera_->Shake(60, 15.0f);
		ChangeState(BossState::Dead);
		return;
	}
#endif

}

void Boss::Draw()
{
	switch (currentState_)
	{
	case BossState::Idle:
		handle_ = graphHandles_[kIdleGraph];
		break;
	case BossState::Attack:
		handle_ = graphHandles_[kAttackGraph];
		break;
	case BossState::Fly:
		handle_ = graphHandles_[kFlyGraph];
		break;
	case BossState::Hurt:
		handle_ = graphHandles_[kHurtGraph];
		break;
	case BossState::Dead:
		handle_ = graphHandles_[kDeadGraph];
		break;
	}

	float drawX = pos_.x + cameraOffset_.x;
	float drawY = pos_.y + cameraOffset_.y;
	//描画
	int animIndex = static_cast<int>(currentState_);
	int frame = animations_[animIndex]->GetFrameCount();

	//アニメーションの描画
	animations_[animIndex]->Draw(drawX, drawY, !isTurn_);
#ifdef _DEBUG
	colRect_.DrawAndCamera(cameraOffset_, 0xff0000, false);
#endif
}

void Boss::ChangeState(BossState nextState)
{
	bool islanding = false;

	if (!isActive_ && nextState != BossState::Idle)
		return;

	BossState prevState = currentState_;
	currentState_ = nextState;
	stateTimer_ = 0;

	// ★ 共通：アニメーション初期化
	animations_[static_cast<int>(nextState)]->Reset();

	if (nextState == BossState::Dead)
	{
		islanding = false;

		//落下開始
		vel_.x = 0.0f;
		vel_.y = 0.0f;

		//行動停止
		isCharging_ = false;
		shotTimer_ = 0.0f;
		return;
	}

	
	if (nextState == BossState::Attack)
	{
		hasShot_ = false;
	}
}

void Boss::UpdateIdle()
{
	stateTimer_++;

	//ノックバックしても元の位置に戻る
	pos_.x += (backPos_.x - pos_.x) * kComeBackPos;
	pos_.y += (backPos_.y - pos_.y) * kComeBackPos;

	float distance = std::abs(pos_.x - pPlayer_->GetPos().x);

	//プレイヤーが近いと攻撃アニメーション
	if (distance < kDistance)
	{
		ChangeState(BossState::Attack);
		return;
	}

	//一定時間経過で飛行アニメーション
	if (stateTimer_ > 60 && rand() % 100 < 2)
	{
		ChangeState(BossState::Fly);
		return;
	}

	//ゆっくり上下移動
	pos_.y += sin(stateTimer_ * 0.1f) * 0.5f;
}

void Boss::UpdateAttack()
{
	stateTimer_++;

	if (isCharging_)
	{
		pos_.x += chargeVel_.x;
		pos_.y += chargeVel_.y;

		// 突進時間終了でFlyへ
		if (stateTimer_ > kRushTime)
		{
			isCharging_ = false;
			ChangeState(BossState::Fly);
		}
		return;
	}

	vel_.x = 0.0f;

	float targetY = pPlayer_->GetPos().y - 120.0f;
	pos_.y += (targetY - pos_.y) * 0.02f;

	// 一定間隔で弾
	shotTimer_ += 1.0f / 60.0f;
	if (shotTimer_ >= 1.5f)
	{
		shotTimer_ = 0.0f;

		Vector2 bulletVel =
		{
			isTurn_ ? -kBulletSpeed : kBulletSpeed,
			0.0f
		};

		pBm_->AddEnemyBullet(pos_, bulletVel);
	}

	// 短時間でFlyに戻る
	if (stateTimer_ > 120)
	{
		ChangeState(BossState::Fly);
	}
}

void Boss::UpdateFly()
{
	stateTimer_++;

	float dx = pPlayer_->GetPos().x - pos_.x;
	float distance = fabsf(dx);
	float speed = kFlySpeed;

	// ===== 逃げ維持 =====
	if (escapeTimer_ > 0)
	{
		escapeTimer_--;
		speed = kEscapeSpeed;
	}
	else
	{
		// 近すぎたら逃げ開始
		if (distance < 300.0f)
		{
			isTurn_ = (dx > 0); // プレイヤーと逆
			escapeTimer_ = 40;
			speed = kEscapeSpeed;
		}
		// 遠すぎたら接近
		else if (distance > 500.0f)
		{
			isTurn_ = (dx < 0);
			speed = kFlySpeed;
		}
	}

	vel_.x = (isTurn_ ? -speed : speed);
	pos_.x += vel_.x;

	//上下の追従は弱め
	float targetY = pPlayer_->GetPos().y - 150.0f;
	pos_.y += (targetY - pos_.y) * 0.02f;

	//羽ばたき
	pos_.y += sin(stateTimer_ * 0.08f) * 1.2f;

	//重なり防止
	if (fabsf(dx) < 30.0f)
	{
		isTurn_ = !isTurn_;
		escapeTimer_ = 40;
	}

	//攻撃へ
	if (stateTimer_ > 60 && rand() % 100 < 2)
	{
		ChangeState(BossState::Attack);
	}
}

void Boss::UpdateHurt()
{
	stateTimer_++;

	pos_.x += (isTurn_ ? kKnockBackPos : -kKnockBackPos);

	pos_.y += sin(stateTimer_ * 0.3f) * 0.3f;

	//タイマーを進めてIdle状態に戻る
	if (animations_[static_cast<int>(BossState::Hurt)]->IsAnimFinished())
	{
		ChangeState(BossState::Idle);
	}
}

void Boss::UpdateDead()
{
	stateTimer_++;
	vel_.y += kGravity; //重力
	pos_.y += vel_.y;

	if (pos_.y > kGround)
	{
		pos_.y = kGround;
		vel_.y = 0.0f;
	}

	//死亡アニメーションが終わったらガチの死亡
	if (stateTimer_ > 0 && animations_[static_cast<int>(BossState::Dead)]->IsAnimFinished())
	{
		isDead_ = true;
	}
}

void Boss::OnHit(int damage)
{
	if (currentState_ == BossState::Dead) return;

	hp_ -= damage;

	pCamera_->Shake(kCameraDuration, kCameraMagnitude);

	if(pPlayer_->GetPos().x < pos_.x)
	{
		knockbackDir_ = 1; //右方向にノックバック
	}
	else
	{
		knockbackDir_ = -1; //左方向にノックバック
	}

	if (hp_ <= 0)
	{
		pCamera_->Shake(90, 35.0f);
		ChangeState(BossState::Dead);
		return;
	}

	//すでにダメージを受けているなら上書をしないようにする
	if (currentState_ != BossState::Hurt)
	{
		ChangeState(BossState::Hurt);
	}

}
