#include "GameScene.h"
#include"Bg.h"
#include"Dog.h"
#include"Rect.h"
#include"Input.h"
#include"Player.h"
#include"Zombie.h"
#include"Boss1.h"
#include"Boss2.h"
#include"GameProgress.h"
#include"Application.h"
#include "SkullFlower.h"
#include "GameOverScene.h"
#include "ClearScene.h"
#include "SelectScene.h"
#include "PauseScene.h"
#include "TutorialManager.h"
#include"SceneController.h"
#include"GlobalConstants.h"
#include "CollisionManager.h"
#include<cmath>
#include<cassert>
#include<Dxlib.h>

namespace
{
	//プレイヤーのスポーン位置
	constexpr float kPlayerSpawnPosX = 200.0f;
	constexpr float kPlayerSpawnPosY = 1744.0f;

	//ボス1のスポーン位置
	constexpr float kBoss1SpawnPosX = 8800.0f;
	constexpr float kBoss1SpawnPosY = 1600.0f;

	//ボス2のスポーン位置
	constexpr float kBoss2SpawnPosX = 8600.0f;
	constexpr float kBoss2SpawnPosY = 1600.0f;

	constexpr Vector2 kDebugWarpPos{ 8300.0f, 1740.0f };

	//フェードまでの間隔
	constexpr int fade_interval = 60;
	constexpr float kColSize = 32.0f;

	//落下判定となる座標
	constexpr float kFallLimit = 2050.0f;

	//プレイヤーが自動で歩行する向きと距離
	constexpr int kPlayerDir = 1;//右向き
	constexpr float kPlayerAutoWalkX = 480.0f;

	//ステージ名を表示する時間
	constexpr int kStageTextDuration = 120;

	//松明解放テキストを表示する時間
	constexpr int kTorchMessageDuration = 180;

	//ライフの描画の拡大率
	constexpr float kHpScale = 2.5f;

	// カメラ揺れ
	constexpr int kBossDefeatShakeFrame = 60;
	constexpr float kBossDefeatShakePower = 6.0f;

	// 自動歩行終了位置
	constexpr float kAutoWalkFinishX = 9050.0f;

	// 松明メッセージ
	constexpr int kTorchMessageEndTime = 180;
	constexpr int kTorchMessageOffsetY = 100;
	constexpr int kMessagePadding = 16;

	constexpr int FadeMaxAlpha = 255;
	constexpr int MessageAlpha = 220;

	constexpr int kHpBaseX = 100;
	constexpr int kHpBaseY = 100;
	constexpr int kHpSpacing = 84;
	constexpr int kMaxHp = 3;

	constexpr int kHpAnimGraphW = 32;
	constexpr int kHpAnimGraphH = 32;
	constexpr int kHpCenterX = 16;
	constexpr int kHpCenterY = 16;
	constexpr int kFrameCount = 13;
	constexpr int kFrameInterval = 6;
	constexpr int kStartX = 0;
	constexpr int kStartY = 160;
}

GameScene::GameScene(SceneController& controller, StageType stage) :
	Scene(controller),
	stageType_(stage),
	update_(&GameScene::FadeInUpdate),
	draw_(&GameScene::FadeDraw),
	clearState_(ClearState::None),
	gameProgress_(nullptr),
	frame_(fade_interval),
	stageTextTimer_(0),
	isTorchUnlockMessageShow_(false),
	isTorchMessageActive_(false),
	torchMesseageTimer_(0),
	hpHandle_(-1),
	fontHandle_(-1),
	fontTorchTextHandle_(-1),
	isBoss1Defeated_(false),
	autoWalkFinishX_(0.0f)
{
	//bgにステージに対応するマップデータをセット
	bg_ = std::make_shared<Bg>(stageType_);
	pCamera_ = std::make_shared<Camera>();
}

GameScene::~GameScene()
{
	DeleteGraph(hpHandle_);
	DeleteFontToHandle(fontHandle_);
	DeleteFontToHandle(fontTorchTextHandle_);
}

void GameScene::FadeInUpdate(Input&)
{
	pCamera_->Update(pPlayer_);

	if (frame_-- <= 0)
	{
		update_ = &GameScene::NormalUpdate;
		draw_ = &GameScene::NormalDraw;
		return;
	}
}

void GameScene::NormalUpdate(Input& input)
{
	//ポーズボタンを押したらポーズシーンに遷移
	if (input.IsTriggered("pause"))
	{
		Application::GetInstance().GetSEManager().PlaySE(SE::Cancel);
		controller_.PushScene(std::make_shared<PauseScene>(controller_));
		return;
	}

	//チュートリアルでテキスト表示中ならゲーム更新を停止させる
	bool isTutorialPaused = false;
	if (tutorialManager_ && !tutorialManager_->IsTutorialFinished())
	{
		tutorialManager_->Update(*pPlayer_, input);
		if (tutorialManager_->IsGamePaused())
		{
			isTutorialPaused = true;
		}
	}

	// ゲーム更新を停止
	if (isTutorialPaused) return;

	//落下判定
	if (pPlayer_->GetPos().y > kFallLimit)
	{
		Application::GetInstance().GetSEManager().PlaySE(SE::Fall);
		controller_.GetProgress().deathCount_++;
		if (stageType_ == StageType::Tutorial)
		{
			controller_.ChangeScene(
				std::make_shared<GameScene>(controller_, StageType::Tutorial));
		}
		update_ = &GameScene::FadeOutUpdate;
		draw_ = &GameScene::FadeDraw;
		frame_ = 0;
		return;
	}

	//各クラスの更新
	pCamera_->Update(pPlayer_);

	pPlayer_->Update(input, bulletManager_, stageType_);

	if (tutorialManager_)
	{
		tutorialManager_->Update(*pPlayer_, input);
	}

	//UIの更新
	weaponUI_.Update(*pPlayer_);

	//敵の更新
	enemyFactory_.Update();

	//弾の更新
	bulletManager_.SetEffectManager(&effectManager_);
	bulletManager_.Update(input, enemyFactory_.GetEnemies(), *pPlayer_);

	//エフェクトの更新
	effectManager_.SetCameraOffset(pCamera_->GetOffset());
	effectManager_.Update();

	//プレイヤーの弾 × 敵の当たり判定(ボスのOnHitは専用なので個別で当たり判定をする)
	CollisionManager::PlayerBulletsVsEnemies(bulletManager_.GetBullets(), enemyFactory_.GetEnemies());

	auto boss2 = enemyFactory_.GetBoss2();
	if (boss2)
	{
		CollisionManager::PlayerBulletsVsBoss2(
			bulletManager_.GetBullets(), *boss2);
	}

	//プレイヤーが死亡していて死亡アニメーションが終了している場合はゲームオーバーシーンへ遷移
	if (pPlayer_->IsDead() && pPlayer_->IsDeadAnimFinished())
	{
		controller_.GetProgress().deathCount_++;
		if (stageType_ == StageType::Tutorial)
		{
			controller_.ChangeScene(
				std::make_shared<GameScene>(controller_, StageType::Tutorial));
		}
		else
		{
			update_ = &GameScene::FadeOutUpdate;
			draw_ = &GameScene::FadeDraw;
			frame_ = 0;
			return;
		}
	}

	bool tookDamage = false;

	//敵弾との判定
	Bullet* hitBullet = CollisionManager::EnemyBulletsVsPlayer(bulletManager_.GetBullets(), *pPlayer_);
	if (hitBullet)
	{
		pPlayer_->OnDamage(hitBullet->GetPos().x);
		hitBullet->OnHit();
		tookDamage = true;
	}

	//敵との当たり判定
	Enemy* hitEnemy = CollisionManager::PlayerVsEnemies(pPlayer_->GetColRect(), enemyFactory_.GetEnemies());
	if (hitEnemy && !pPlayer_->IsDead())
	{
		pPlayer_->OnDamage(hitEnemy->GetPos().x);
		tookDamage = true;
	}

	auto boss1 = enemyFactory_.GetBoss1();
	if (boss1 && boss1->IsDeadAnimFinished() && clearState_ == ClearState::None)
	{
		clearState_ = ClearState::BossCameraShake;

		// ボス1撃破フラグ
		controller_.GetProgress().isDefeatedBoss1_ = true;

		// カメラ揺れ
		pCamera_->Shake(kBossDefeatShakeFrame, kBossDefeatShakePower);
	}

	//プレイヤーとボス2の当たり判定
	if (boss2)
	{
		if (!pPlayer_->IsDead())
		{
			if (CollisionManager::PlayerVsBoss2(
				pPlayer_->GetColRect(), *boss2))
			{
				pPlayer_->OnDamage(boss2->GetPos().x);
				tookDamage = true;
			}
		}

		//ボスのパンチとプレイヤーの当たり判定
		if (boss2->IsPunchActive() && !boss2->HasPunchHit())
		{
			if (boss2->GetPunchRect().IsCollision(pPlayer_->GetColRect()))
			{
				pPlayer_->OnDamage(boss2->GetPos().x);
				tookDamage = true;

				// このパンチではもう当たらない
				boss2->SetPunchHit();
			}
		}
	}

	//HPUI更新
	if (tookDamage)
	{
		OnDamagedHpUI();
	}

	//ボスを倒すとクリアシーンに遷移する
	if (boss2)
	{
		//ボスが死亡アニメ終了していたらクリア処理開始
		if (boss2->IsDeadAnimFinished() && clearState_ == ClearState::None)
		{
			controller_.ChangeScene(std::make_shared<ClearScene>(controller_));
			return;
		}
	}

	//クリア状態がボスを倒した後のカメラ揺らし状態の場合は
	//カメラの揺れが終了するまで待機する
	if (clearState_ == ClearState::BossCameraShake)
	{
		if (!pCamera_->IsShaking())
		{
			clearState_ = ClearState::AutoWalk;
			autoWalkFinishX_ = kAutoWalkFinishX;
			pPlayer_->StartAutoWalk(kPlayerDir);
		}
	}

	//クリア状態が自動歩行状態の場合は
	//一定距離進んだらクリアシーンへ遷移する
	if (clearState_ == ClearState::AutoWalk)
	{
		if (pPlayer_->GetPos().x >= autoWalkFinishX_)
		{
			update_ = &GameScene::GoalFadeOutUpdate;
			draw_ = &GameScene::FadeDraw;
			frame_ = 0;
			return;
		}
	}

	//チュートリアルステージのゴール判定処理
	if (tutorialManager_)
	{
		const auto& goalRect = tutorialManager_->GetGoalRect();
		const auto& playerRect = pPlayer_->GetColRect();

		if (goalRect.IsCollision(playerRect))
		{
			Application::GetInstance().GetSEManager().PlaySE(SE::Warp);
			//ゴール到達したらフェードアウトして次のシーンへ遷移
			update_ = &GameScene::GoalFadeOutUpdate;
			draw_ = &GameScene::FadeDraw;
			frame_ = 0;
			return;
		}
	}

	//ステージテキストを表示するタイマーのカウントダウン
	if (stageTextTimer_ > 0)
	{
		stageTextTimer_--;
	}

	//ステージ名の表示のあとに松明解放テキスト表示
	if (isTorchMessageActive_ && stageTextTimer_ <= 0)
	{
		if (!isTorchUnlockMessageShow_)
		{
			Application::GetInstance().GetSEManager().PlaySE(SE::UnlockTorch);
			isTorchUnlockMessageShow_ = true;
		}
		torchMesseageTimer_++;
		if (torchMesseageTimer_ >= kTorchMessageEndTime)
		{
			isTorchMessageActive_ = false;
		}
	}
	//HPUIの更新
	for (auto& hp : hpUIs_)
	{
		if (hp.isBroken)
		{
			hp.anim->Update();
		}
	}

#ifdef _DEBUG
	//デバッグ用：ステージクリア
	if (input.IsTriggered("debug_warp"))
	{
		pPlayer_->SetPos(kDebugWarpPos);
	}
#endif
}

void GameScene::FadeOutUpdate(Input&)
{
	for (auto& hp : hpUIs_)
	{
		if (hp.isBroken)
			hp.anim->Update();
	}

	//HPUIが終わったかどうかを返すフラグ
	bool isHpAnimFinished = true;
	for (auto& hp : hpUIs_)
	{
		if (hp.isBroken && !hp.anim->IsFinished())
		{
			isHpAnimFinished = false;
			break;
		}
	}

	if (!isHpAnimFinished)
	{
		return;
	}

	if (frame_++ >= fade_interval)
	{
		if (stageType_ == StageType::Tutorial)
		{
			controller_.ChangeScene(std::make_shared<GameOverScene>(controller_, StageType::Tutorial));
		}
		else
		{
			controller_.ChangeScene(std::make_shared<GameOverScene>(controller_, stageType_));
		}
		return;
	}
}

void GameScene::GoalFadeOutUpdate(Input&)
{
	if (frame_++ >= fade_interval)
	{
		//次のステージを取得
		StageType next = GetNextStageType(stageType_);
		controller_.ChangeScene(std::make_shared<GameScene>(controller_, next));
	}
}

void GameScene::FadeDraw()
{
	NormalDraw();

	//フェードの描画
	auto rate = static_cast<float>(frame_) / static_cast<float>(fade_interval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast <int>(FadeMaxAlpha * rate));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);//ブレンドしない
}

void GameScene::NormalDraw()
{
	Vector2 cameraOffset = pCamera_->GetOffset();

	//各クラスの描画処理
	bg_->Draw(pCamera_);
	pPlayer_->SetCameraOffset(cameraOffset);
	pPlayer_->Draw();

	enemyFactory_.Draw(cameraOffset);

	bulletManager_.SetCameraOffset(cameraOffset);
	bulletManager_.Draw();

	effectManager_.SetCameraOffset(cameraOffset);
	effectManager_.Draw();

	//UIの描画
	weaponUI_.Draw();

	DrawHpUI();

	//チュートリアルステージの描画
	if (tutorialManager_)
	{
		tutorialManager_->Draw(*pCamera_);
	}

	//ステージ名の描画
	DrawStageText();

	if (isTorchMessageActive_ && stageTextTimer_ <= 0)
	{
		const char* message = "たいまつを解放しました！";

		//文字幅を取得して中央に表示
		int msgWidth = GetDrawStringWidthToHandle(message, static_cast<int>(strlen(message)), fontTorchTextHandle_);
		int msgHeight = GetFontSizeToHandle(fontTorchTextHandle_);
		int x = (Game::kScreenWidth - msgWidth) / 2;
		int y = Game::kScreenHeight / 2 - kTorchMessageOffsetY;

		int boxX1 = x - kMessagePadding;
		int boxY1 = y - kMessagePadding;
		int boxX2 = x + msgWidth + kMessagePadding;
		int boxY2 = y + msgHeight + kMessagePadding;

		//フェードイン・フェードアウト演出

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, MessageAlpha);
		DrawBox(boxX1, boxY1, boxX2, boxY2, 0x000000, TRUE);
		DrawStringToHandle(x, y, message, 0xffff00, fontTorchTextHandle_);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void GameScene::DrawHpUI()
{
	for (int i = 0; i < kMaxHp; i++)
	{
		float x = static_cast<float>(kHpBaseX + i * kHpSpacing);
		float y = static_cast<float>(kHpBaseY);

		if (hpUIs_[i].isBroken)
		{
			hpUIs_[i].anim->Draw(x, y, false);
		}
		else
		{
			DrawRectRotaGraph3(
				(int)x, (int)y,
				0, 0,
				kHpAnimGraphW, kHpAnimGraphH,
				kHpCenterX, kHpCenterY,
				kHpScale, kHpScale,
				0.0f,
				hpHandle_,
				TRUE,
				FALSE);
		}
	}
}

void GameScene::DrawStageText()
{
	if (stageTextTimer_ <= 0) return;

	const char* stageText = "";

	switch (stageType_)
	{
	case StageType::Tutorial:
		stageText = "TUTORIAL";
		break;
	case StageType::Stage1:
		stageText = "STAGE 1";
		break;
	case StageType::Stage2:
		stageText = "STAGE 2";
		break;
	case StageType::Stage3:
		stageText = "STAGE 3";
		break;
	default:
		stageText = "";
		break;
	}

	//フェードアウト
	float rate = static_cast<float>(stageTextTimer_) / kStageTextDuration;
	int alpha = static_cast<int>(FadeMaxAlpha * rate);

	int stageTextW = GetDrawStringWidthToHandle(stageText, static_cast<int>(size_t(stageText)), fontHandle_);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

	DrawStringToHandle(
		(Game::kScreenWidth / 2 - stageTextW / 2),
		Game::kScreenHeight / 2,
		stageText,
		0xff0000,
		fontHandle_);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameScene::Init()
{
	hpHandle_ = LoadGraph("data/UI/Life.png");
	fontHandle_ = CreateFontToHandle("g_コミックホラー悪党-教漢", 72, -1, -1);
	fontTorchTextHandle_ = CreateFontToHandle("g_コミックホラー悪党-教漢", 24, -1, -1);

	//ゲーム進行状況の取得
	gameProgress_ = &controller_.GetProgress();

	//各クラスの初期化
	bg_ = std::make_shared<Bg>(stageType_);
	bg_->Init();

	//プレイヤーの初期化
	pPlayer_ = std::make_shared<Player>(Vector2{ kPlayerSpawnPosX, kPlayerSpawnPosY }, Vector2{ 0.0f,0.0f });
	pPlayer_->Init();
	pPlayer_->SetCamera(pCamera_);
	pPlayer_->SetBg(bg_);
	pPlayer_->SetGameProgress(gameProgress_);

	//カメラの初期化
	pCamera_->Init(pPlayer_);

	bulletManager_.SetCamera(pCamera_.get());

	//敵の初期化
	enemyFactory_.SetEffectManager(&effectManager_);
	enemyFactory_.LoadFromCSV(stageType_, &bulletManager_);
	//チュートリアルステージ用の初期化
	if (stageType_ == StageType::Tutorial)
	{
		tutorialManager_ = std::make_unique<TutorialManager>();
		tutorialManager_->Init();
	}
	else
	{
		tutorialManager_.reset();
	}
	if (stageType_ == StageType::Stage1)
	{
		enemyFactory_.AddBoss1(Vector2{ kBoss1SpawnPosX,kBoss1SpawnPosY }, Vector2{ 0.0f,0.0f },
			pPlayer_, &bulletManager_, pCamera_, Boss1Type::Normal);
	}
	else if (stageType_ == StageType::Stage2)
	{
		enemyFactory_.AddBoss1(Vector2{ kBoss1SpawnPosX,kBoss1SpawnPosY }, Vector2{ 0.0f,0.0f },
			pPlayer_, &bulletManager_, pCamera_, Boss1Type::Variant);
	}
	else if (stageType_ == StageType::Stage3)
	{
		enemyFactory_.AddBoss2(Vector2{ kBoss2SpawnPosX,kBoss2SpawnPosY }, Vector2{ 0.0f,0.0f },
			pPlayer_, &bulletManager_, pCamera_);
	}
	else if (stageType_ == StageType::BossDebugStage)
	{
		enemyFactory_.AddBoss2(Vector2{ kBoss2SpawnPosX, kBoss2SpawnPosY }, Vector2{ 0.0f,0.0f },
			pPlayer_, &bulletManager_, pCamera_);
	}
	enemyFactory_.Init(pPlayer_, bg_);

	//UIの初期化
	weaponUI_.Init();

	//BGMの再生
	Application::GetInstance().GetBGMManager().PlayBGM(BGM::Game);

	//たいまつのアンロックするための処理
	auto& progress = controller_.GetProgress();
	if (progress.isDefeatedBoss1_)
	{
		pPlayer_->UnlockTorch();
		if (stageType_ == StageType::Stage2)
		{
			isTorchMessageActive_ = true;
			isTorchUnlockMessageShow_ = false;
		}
	}
	stageTextTimer_ = kStageTextDuration;

	for (int i = 0; i < kMaxHp; i++)
	{
		hpUIs_[i].isBroken = false;
		hpUIs_[i].anim = std::make_unique<SpriteAnimation>(
			hpHandle_,
			kHpAnimGraphW, kHpAnimGraphH,
			kFrameCount,
			kFrameInterval,
			kHpScale,
			kStartX, kStartY,
			false);
	}
}

void GameScene::Update(Input& input)
{
	(this->*update_)(input);
}

void GameScene::Draw()
{
	(this->*draw_)();
}

void GameScene::OnDamagedHpUI()
{
	int hp = pPlayer_->GetHp();

	for (int i = kMaxHp - 1; i >= 0; i--)
	{
		if (i >= hp)
		{
			if (!hpUIs_[i].isBroken)
			{
				hpUIs_[i].isBroken = true;
				hpUIs_[i].anim->Reset();
			}
		}
	}
}

//次のステージタイプを取得
StageType GameScene::GetNextStageType(StageType nextStage)
{
	switch (nextStage)
	{
	case StageType::Tutorial:
		return StageType::Stage1;
	case StageType::Stage1:
		return StageType::Stage2;
	case StageType::Stage2:
		return StageType::Stage3;
	default:
		return nextStage;
	}
}