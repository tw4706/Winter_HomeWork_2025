#pragma once
#include <vector>
#include <memory>
#include "Enemy.h"
#include "Player.h"
#include "Bg.h"

enum class StageType;
class GameScene;
class EnemyFactory
{

public:
    void LoadFromCSV(StageType stageType, BulletManager* bulletManager);
    void Init(std::shared_ptr<Player> player, std::shared_ptr<Bg> bg);
    void Update();
    void Draw(const Vector2& cameraOffset);
    std::vector<std::shared_ptr<Enemy>>& GetEnemies() { return enemies_; }

    void AddBoss1(Vector2 pos, Vector2 vel,
        std::shared_ptr<Player>player, BulletManager* bm, std::shared_ptr<Camera>camera);

    void AddBoss2(Vector2 pos, Vector2 vel,
		std::shared_ptr<Player>player,
        BulletManager* bm, std::shared_ptr<Camera>camera, BulletType playerWeapon);
private:
    std::vector<std::shared_ptr<Enemy>> enemies_;
    StageType stageType_;
};

