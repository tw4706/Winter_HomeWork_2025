#pragma once
#include"Character.h"
class Input;
class Player :public Character
{
public:
	Player();
	~Player()override;

	virtual void Init()override;
	virtual void Update()override;
	virtual void Update(Input&input);
	virtual void Draw()override;

	void Move(Input& input);
	void Jump();
private:
	int playerH_;//プレイヤーの画像ハンドル
};

