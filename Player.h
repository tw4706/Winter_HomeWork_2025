#pragma once
#include"GameObject.h"
#include"Enemy.h"
#include<vector>
#include<memory>
class Input;
class BulletManager;
class Player :public GameObject
{
public:
	Player(Vector2 pos, Vector2 vel);
	~Player()override;

	virtual void Init()override;
	virtual void Update()override;
	void Update(Input& input,BulletManager&bm );
	virtual void Draw()override;

	void Move(Input& input);
	void Jump(Input& input);
	Vector2 GetPos()const;//プレイヤーの位置を取得する


private:
	int playerH_;//プレイヤーの画像ハンドル
	bool isJumping_;//ジャンプしているかどうか
	bool canDoubleJumping_;//ダブルジャンプ可能かどうか
};

