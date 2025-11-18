#pragma once
#include"GameObject.h"
#include"Geometry.h"
#include<memory>
#include<vector>

//プレイヤーの玉の種別
enum class PlayerBulletType
{
	Bullet,
	Lance,
	Torch
};

//弾の情報
struct BulletConfig
{
	const char* imagePath;
	int width;
	int height;
};

class Input;
class Enemy;
class Bullet:public GameObject
{
public:
	Bullet(Vector2 pos,Vector2 vel,PlayerBulletType bulletType);
	~Bullet()override;

	void Init()override;
	void Update()override;
	void UpdateShot();
	void Update(Input&input,std::vector<std::shared_ptr<Enemy>>& enemies);
	void Draw()override;

	//当たった時の処理を行う関数
	void OnHit();

	// 弾が消えたかどうかを返す
	void Destroy() { isAlive_ = false; }
	bool IsAlive() const { return isAlive_; }
	//弾の種類を返す
	PlayerBulletType GetType() const { return bulletType_; }

private:
	Vector2 pos_;		//位置
	Vector2 vel_;		//速度
	bool isAlive_;		//弾が存在しているかどうか
	int bulletH_;		//弾の画像ハンドル
	PlayerBulletType bulletType_;
};

