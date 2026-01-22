#pragma once
#include <vector>
#include <memory>
#include "Enemy.h"
#include "Player.h"
#include"StageType.h"
#include "Bg.h"
#include "Boss1.h"

class GameScene;
class Boss2;
class EffectManager;
class EnemyFactory
{
public:
    void LoadFromCSV(StageType stageType, BulletManager* bulletManager);
    void Init(std::shared_ptr<Player> player, std::shared_ptr<Bg> bg);
    void Update();
    void Draw(const Vector2& cameraOffset);
    std::vector<std::shared_ptr<Enemy>>& GetEnemies() { return enemies_; }

    void AddBoss1(Vector2 pos, Vector2 vel,
        std::shared_ptr<Player>player, BulletManager* bm, std::shared_ptr<Camera>camera, Boss1Type type);

    void AddBoss2(Vector2 pos, Vector2 vel,
		std::shared_ptr<Player>player,
        BulletManager* bm, std::shared_ptr<Camera>camera);

    std::shared_ptr<Boss1> GetBoss1() const { return boss1_; }
    std::shared_ptr<Boss2> GetBoss2() const { return boss2_; }

    void SetEffectManager(EffectManager* effectManager);

private:
    std::vector<std::shared_ptr<Enemy>> enemies_;
    std::shared_ptr<Boss1> boss1_ = nullptr;
    std::shared_ptr<Boss2> boss2_ = nullptr;
    StageType stageType_;
    EffectManager* pEffectManager_=nullptr;//エフェクトマネージャのポインタ
};

