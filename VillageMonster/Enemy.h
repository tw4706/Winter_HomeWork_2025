#pragma once
class Enemy
{
public:
	Enemy();
	~Enemy();

	void Init();
	void Update();
	void Draw();
	virtual void Attack() = 0;
private:

};

