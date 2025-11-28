#pragma once
#include"Rect.h"
class CollisionManager
{
public:
	CollisionManager();
	~CollisionManager();
	static void ResolveCollision(Rect&rect, Vector2& pos, Vector2& vel,const Rect&chipRect);
private:
};

