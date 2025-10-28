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
private:
	int playerH_;//プレイヤーの画像ハンドル
};

