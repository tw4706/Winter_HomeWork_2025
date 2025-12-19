#include "EnemyFactory.h"
#include "Zombie.h"
#include "SkullFlower.h"
#include "Dog.h"
#include "Boss.h"
#include "GameScene.h"
#include<Dxlib.h>
#include <fstream>
#include <sstream>
#include <map>
#include <functional>

namespace
{
    //セルのサイズ
	const int cellSize = 48;

	//敵の描画オフセット
    constexpr float kEnemyOffsetY = 32.0f;
}

void EnemyFactory::LoadFromCSV(StageType stageType, BulletManager* bulletManager)
{

    std::string path;
    switch (stageType)
    {
    case StageType::Stage1:
        path = "data/Enemy/enemyData.csv";
        break;
    case StageType::Stage2:
        path = "data/Enemy/enemyData2.csv";
        break;
    default:
        path = "data/Enemy/enemyData.csv";
        break;
    }

    std::ifstream file(path);
    std::string line;
    int row = 0;

    //敵生成用ファクトリ
    std::map<int, std::function<std::shared_ptr<Enemy>(Vector2)>> enemyFactory = 
    {
        {1, [](Vector2 pos) { return std::make_shared<Zombie>(pos, Vector2{0,0}); }},
        {2, [bulletManager](Vector2 pos) { return std::make_shared<SkullFlower>(pos, Vector2{0,0}, bulletManager); }},
        {3, [](Vector2 pos) { return std::make_shared<Dog>(pos, Vector2{0,0}); }}
    };

	//既存の敵データを削除
    enemies_.clear();

	//CSVファイルの各行を読み込み
    while (std::getline(file, line)) 
    {
        std::stringstream ss(line);
        std::string cell;
        int col = 0;

        while (std::getline(ss, cell, ','))
        {
            int id = std::stoi(cell);
            if (id != 0 && enemyFactory.count(id)) 
            {
                Vector2 pos{ col * cellSize, row * cellSize+kEnemyOffsetY };
                enemies_.push_back(enemyFactory[id](pos));
            }
            col++;
        }
        row++;
    }
}


void EnemyFactory::Init(std::shared_ptr<Player> player, std::shared_ptr<Bg> bg)
{
	//敵の初期化
    for (auto& enemy : enemies_) 
    {
        enemy->Init();
        enemy->SetPlayer(player);
        enemy->SetBg(bg);
    }
}

void EnemyFactory::Update()
{
	//敵の更新
    for (auto& enemy : enemies_) 
    {
        if (!enemy->IsDead()) 
        {
            enemy->Update();
        }
    }
}

void EnemyFactory::Draw(const Vector2& cameraOffset)
{
	//敵の描画
    for (auto& enemy : enemies_) 
    {
        if (!enemy->IsDead()) 
        {
            enemy->SetCameraOffset(cameraOffset);
            enemy->Draw();
        }
    }
}

void EnemyFactory::AddBoss(Vector2 pos, Vector2 vel,
    std::shared_ptr<Player>player,BulletManager*bm, std::shared_ptr<Camera>camera)
{
    auto boss = std::make_shared<Boss>(pos, vel,player,bm,camera);
    boss->Init();
    boss->SetPlayer(player);
    enemies_.push_back(boss);
}

