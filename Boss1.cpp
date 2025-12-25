#include "Boss1.h"
#include "Player.h"
#include "BulletManager.h"

namespace 
{
	//近づかれすぎた時の逃げる速度
	constexpr float kComeBackPos = 0.5f;
	constexpr float kDistance = 400.0f;

	constexpr float kFlySpeed = 0.4f;
	constexpr float kEscapeSpeed = 1.8f;
	constexpr float kBulletSpeed = 3.0f;
	constexpr float kShotInterval = 5.0f;
	constexpr int KMaxHp = 30;
	constexpr float kRushSpeed = 4.5f;   //突進する速度
	constexpr int   kRushTime = 18;     //突進するフレーム数
}

Boss1::Boss1(Vector2 pos, Vector2 vel,
	std::shared_ptr<Player> player,
	BulletManager* bm,
	std::shared_ptr<Camera> camera)
	:Boss(pos, vel, player, bm, camera)
{
}

void Boss1::Init()
{
	Boss::Init();
}

void Boss1::UpdateIdle()
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

void Boss1::UpdateAttack()
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

void Boss1::UpdateMove()
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
