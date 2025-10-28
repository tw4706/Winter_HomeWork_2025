#pragma once
#include"GameObject.h"
class Input;
class Player :public GameObject
{
public:
	Player();
	~Player()override;

	void Init()override;
	void Update()override;
	void Update(Input&input);
	void Draw()override;
	void Gravity(Input&input);
	void Jump(Input&input);
private:
	int frameCount_;//カウント
	int playerH_;//プレイヤーの画像ハンドル
	bool isTurn_;//画像が反転してるかどうか
	bool isJumping_;//ジャンプしているかどうか
};

