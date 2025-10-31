#pragma once
#include"GameObject.h"
#include"Rect.h"
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
	int width_;//•
	int height_;//‚‚³
	bool isTurn_;//”½“]‚µ‚Ä‚é‚©‚Ç‚¤‚©
	bool isGround_;//’n–Ê‚ÉÚ’n‚µ‚Ä‚¢‚é‚©‚Ç‚¤‚©
	Rect colRect_;//“–‚½‚è”»’è‚Ì‹éŒ`
};

