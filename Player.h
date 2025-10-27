#pragma once
class Input;
class Player
{
public: 
	Player();
	~Player();

	void Init();
	void Update(Input&input);
	void Draw();

private:
	int playerHandle_;//プレイヤーの画像ハンドル
	float x_;
	float y_;
};

