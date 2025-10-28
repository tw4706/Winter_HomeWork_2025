#pragma once
#include"GameObject.h"
class Character:public GameObject
{
public:
	Character();
	virtual ~Character();
	virtual void Init() = 0;
	virtual void Update();
	virtual void Draw() = 0;
protected:
	virtual void Gravity();
protected:
	int jump_frame;//ジャンプしているフレーム数
	bool is_turn;//反転してるかどうか
	bool is_ground;//地面に接地しているかどうか
};

