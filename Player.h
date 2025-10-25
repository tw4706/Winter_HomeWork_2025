#pragma once
class Input;
class Geometory;
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
protected:
	Geometory pos_;//プレイヤーの位置情報
};

