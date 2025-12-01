#include "CollisionManager.h"
#include<cmath>

CollisionManager::CollisionManager()
{
}

CollisionManager::~CollisionManager()
{
}

void CollisionManager::ResolveCollision(Rect& rect, Vector2& pos,Vector2& vel, 
    const Rect& chipRect)
{

    bool overlapX = (rect.right_ > chipRect.left_ && rect.left_ < chipRect.right_);
    bool overlapY = (rect.bottom_ > chipRect.top_ && rect.top_ < chipRect.bottom_);

    //•Ç‚Ì“–‚½‚è”»’è
    if (overlapY && vel.x != 0 && !(vel.y > 0))
    {
        if (vel.x > 0) 
        {
            pos.x = chipRect.left_ - (rect.GetWidth() / 2.0f);
            vel.x = 0;
        }
        else if (vel.x < 0) 
        {
            pos.x = chipRect.right_ + (rect.GetWidth() / 2.0f);
            vel.x = 0;
        }
    }

    rect.SetCenter(pos.x, pos.y, rect.GetWidth(), rect.GetHeight());

    //°‚â“Vˆä‚Ì”»’è
    if (overlapX && vel.y != 0)
    {
        if (vel.y > 0) 
        {
            pos.y = chipRect.top_ - (rect.GetHeight() / 2.0f);
            vel.y = 0;
        }
        else if (vel.y < 0) 
        {
            pos.y = chipRect.bottom_ + (rect.GetHeight() / 2.0f);
        }
    }
}
