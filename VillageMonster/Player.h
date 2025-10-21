#pragma once
class Player
{
public: 
	Player();
	~Player();

	void Init();
	void Update();
	void Draw();

private:
	float x_;
	float y_;
};

