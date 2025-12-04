#include "EnemyFactory.h"
#include "Zombie.h"
#include "SkullFlower.h"
#include "Dog.h"
#include<Dxlib.h>
#include <fstream>
#include <sstream>
#include <map>
#include <functional>

namespace
{
	const int cellSize = 48; //セルのサイズ
}

void EnemyFactory::LoadFromCSV(const std::string& path, BulletManager* bulletManager)
{
    std::ifstream file(path);
    std::string line;
    int row = 0;

    // 敵生成用ファクトリ
    std::map<int, std::function<std::shared_ptr<Enemy>(Vector2)>> enemyFactory = 
    {
        {1, [](Vector2 pos) { return std::make_shared<Zombie>(pos, Vector2{}); }},
        {2, [bulletManager](Vector2 pos) { return std::make_shared<SkullFlower>(pos, Vector2{}, bulletManager); }},
        {3, [](Vector2 pos) { return std::make_shared<Dog>(pos, Vector2{}); }}
    };

	//CSVファイルの各行を読み込み
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        int col = 0;

        while (std::getline(ss, cell, ','))
        {
            int id = std::stoi(cell);
            if (id != 0 && enemyFactory.count(id)) {
                Vector2 pos{ col * cellSize, row * cellSize };
                enemies_.push_back(enemyFactory[id](pos));
            }
            col++;
        }
        row++;
    }
}


void EnemyFactory::Init(std::shared_ptr<Player> player, std::shared_ptr<Bg> bg)
{
	// 敵の初期化
    for (auto& enemy : enemies_) 
    {
        enemy->Init();
        enemy->SetPlayer(player);
        enemy->SetBg(bg);
    }
}

void EnemyFactory::Update()
{
	// 敵の更新
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
    for (auto& enemy : enemies_)
    {
        Vector2 pos = enemy->GetPos();
        printfDx("Enemy pos: %f, %f\n", pos.x, pos.y);
    }
	// 敵の描画
    for (auto& enemy : enemies_) 
    {
        if (!enemy->IsDead()) 
        {
            enemy->SetCameraOffset(cameraOffset);
            enemy->Draw();
        }
    }
}

