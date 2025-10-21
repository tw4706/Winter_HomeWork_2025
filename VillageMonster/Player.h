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
	float x_;
	float y_;
};

