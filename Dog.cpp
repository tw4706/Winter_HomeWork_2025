#include "Dog.h"
#include "Player.h"
#include<Dxlib.h>
#include<cmath>

namespace
{
	//画像の切り取りサイズ
	constexpr int kGraphW = 48;
	constexpr int kGraphH = 48;
	constexpr int kGraphHalf_W = kGraphW-16;
	constexpr int kGraphHalf_H = kGraphH-16;

	//当たり判定のサイズ
	constexpr  int kRectWidth = 32;
	constexpr  int kRectHeight = 32;

	//ジャンプの高さ
	constexpr float kJumpPower = 15.0f;
	//移動速度
	constexpr float kSpeed = 4.0f;
	//地面の位置
	constexpr float kGround = 450.0f;
	//ジャンプ間隔
	constexpr int kJumpInterval = 120;
	//プレイヤーとの距離
	const float kDistance = 300.0f;
}

Dog::Dog(Vector2 pos, Vector2 vel) :
	Enemy(pos, vel),
	dogH_(-1),
	timer_(0.0f)
{
}

Dog::~Dog()
{
}

void Dog::Init()
{
	dogH_ = LoadGraph("data/Enemy/dog.png");
}

void Dog::Update()
{
	Enemy::Update();
	Move();//移動処理
	pos_ += vel_;

	colRect_.SetCenter(pos_.x,pos_.y, kRectWidth, kRectHeight);

	GameObject::Gravity();

	//地面の接地判定
	if (pos_.y >= kGround)
	{
		pos_.y = kGround;
		vel_.y = 0.0f;
		isGround_ = true;
	}

	DrawFormatString(0, 150, 0xffffff, "Dog PosX:%f", pos_.x);
	DrawFormatString(0, 170, 0xffffff, "Dog VelX:%f", vel_.x);
	DrawFormatString(0, 190, 0xffffff, "Dog Dist:%f", std::abs(pPlayer_->GetPos().x - pos_.x));
}

void Dog::Draw()
{
	DrawRectRotaGraph3(static_cast<int>(pos_.x),
		static_cast<int>(pos_.y),
		0, 0,
		kGraphW,kGraphH,
		kGraphHalf_W, kGraphHalf_H,
		1.5, 1.5,
		0,
		dogH_, true);



#ifdef _DEBUG
	colRect_.Draw(0xff0000, false);
#endif

}

void Dog::Attack()
{

}

void Dog::Move()
{
	timer_++;
	// プレイヤーがセットされていない場合は何もしない
	if (!pPlayer_) return;

	//プレイヤーとの距離を見て移動する処理を追加
	float playerX = pPlayer_->GetPos().x;
	float enemyX = pos_.x;
	float dx = playerX - enemyX;
	float distance = std::abs(dx);
	//動く向きを決める
	int moveDirection = 0;
	//距離が一定以下の時はプレイヤーに向かって移動
	if (distance < kDistance)
	{
		if (pos_.y >= kGround && timer_ > kJumpInterval)
		{
			vel_.y = -kJumpPower;
			timer_ = 0.0f;


			if (dx > 0)
			{
				vel_.x = kSpeed; //プレイヤーが右だと右にジャンプ
			}
			else if (dx < 0)
			{
				vel_.x = -kSpeed; //プレイヤーが左だと左にジャンプ
			}
			else
			{
				vel_.x = 0.0f; //上にジャンプ
			}
		}
		else if (pos_.y >= kGround)
		{
			//地面にいるがジャンプ条件を満たしていない場合は停止
			vel_.x = 0.0f;
		}
	}
	else
	{
		//プレイヤーが遠いときは停止
		vel_.x = 0.0f;
	}
}




