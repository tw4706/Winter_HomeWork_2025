#pragma once
#include"GameObject.h"
#include"Geometry.h"
#include"Animation.h"
#include<memory>
#include<vector>

//プレイヤーの玉の種別
enum class BulletType
{
	Knife,
	Lance,
	Torch,
	EnemyBullet
};

//弾の情報
struct BulletConfig
{
	const char* imagePath;	//画像
	int width;				//幅
	int height;				//高さ
	float speed;			//弾速
	float shotInterval;		//連射間隔
	bool isPiercing;		//貫通するかどうか
};

//波動の情報
struct Hadou
{
	Rect rect_;
	int appearTimer_; //出現までの待機フレーム
	int lifetime;    //出現後の寿命

	std::unique_ptr<Animation> animations_;//波動のアニメーション

	Hadou(const Rect& rect, int appearTimer, int lifetime)
		: rect_(rect),
		appearTimer_(appearTimer),
		lifetime(lifetime),
		animations_(nullptr)
	{}

};

//弾の詳細情報の配列
constexpr BulletConfig kBulletConfigs[] =
{
	{"data/Bullet/Knife.png",32,32,10.0f,15.0f,true},		//槍
	{"data/Bullet/Lance.png",32,32,6.0f,35.0f,false},		//短剣
	{"data/Bullet/Torch.png",32,32,6.0f,20.0f,false},		//たいまつ
	{"data/Bullet/EnemyBullet.png",32,32,6.0f,0.0f,false}	//敵の弾
};

class Input;
class Enemy;
class EffectManager;
class Bullet:public GameObject
{
public:
	Bullet(Vector2 pos,Vector2 vel,BulletType bulletType, std::shared_ptr<Bg>bg);
	~Bullet()override;

	void Init()override;
	void Update() override {};
	void UpdateShot();
	void Update(Input&input,std::vector<std::shared_ptr<Enemy>>& enemies);
	void Draw()override;

	//当たった時の処理を行う関数
	void OnHit();

	//波動の生成
	void SpawnHadou();

	//波動の更新処理
	void UpdateHadou(std::vector<std::shared_ptr<Enemy>>& enemies);

	// 弾が消えたかどうかを返す
	bool IsAlive() const { return isAlive_; }

	//弾の種類を返す
	const BulletType& GetType() const { return bulletType_; }

	//当たった数を増やす関数
	void RegisterHit();

	//たいまつの波動とマップの当たり判定を行う関数
	void CheckBulletAndMapCollision();

	//プレイヤーの弾かどうかを返す関数
	bool IsPlayerBullet() const;

	//弾の向きをセットする関数
	void SetDirection(bool isRight);

	//画面外に出たかどうかを返す関数
	bool IsOutOfScreen() const;

	//ダメージ量を返す関数
	int GetDamage() const;

	//エフェクトマネージャーのセット
	void SetEffectManager(EffectManager* effect);

private:
	bool isAlive_;		//弾が存在しているかどうか
	int bulletH_;		//弾の画像ハンドル
	int damage_;		//弾のダメージ量
	int hitCount_;		//当たった数
	std::vector<int>hadouTimers_;
	BulletType bulletType_;
	std::shared_ptr<Bg> pBg_;
	std::shared_ptr<Animation> animations_;//弾のアニメーション
	EffectManager* pEffectManager_;//エフェクトマネージャーのポインタ
	
	//波動関連
	int hadouH_;
	float hadouDir_;
	float prevHadouDir_;			//波動の向きの保存用
	bool isHadouSpawned_;			//波動を生成したかどうか
	std::vector<Hadou> hadouRects_;	//波動の当たり判定用の矩形
};

