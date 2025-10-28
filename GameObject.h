#pragma once
#include"Geometry.h"
class GameObject
{
public:
	GameObject();
	virtual~GameObject();

	virtual void Init()abstract;
	virtual void Update()abstract;
	virtual void Draw()abstract;
protected:
	Vector2 pos_;
	Vector2 vel_;
};

