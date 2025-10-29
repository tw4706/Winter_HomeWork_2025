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
	bool is_turn;//”½“]‚µ‚Ä‚é‚©‚Ç‚¤‚©
	bool is_ground;//’n–Ê‚ÉÚ’n‚µ‚Ä‚¢‚é‚©‚Ç‚¤‚©
};

