#pragma once
#include <vector>
#include <memory>
#include "Enemy.h"
#include "Player.h"
#include "Bg.h"

class EnemyFactory
{

public:
    void LoadFromCSV(const std::string& path, BulletManager* bulletManager);
    void Init(std::shared_ptr<Player> player, std::shared_ptr<Bg> bg);
    void Update();
    void Draw(const Vector2& cameraOffset);
    std::vector<std::shared_ptr<Enemy>>& GetEnemies() { return enemies_; }

private:
    std::vector<std::shared_ptr<Enemy>> enemies_;
};

