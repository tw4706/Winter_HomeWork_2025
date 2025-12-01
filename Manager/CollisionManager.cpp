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

        // è’ìÀîªíË
    if (rect.right_ > chipRect.left_ && rect.left_ < chipRect.right_ &&
        rect.bottom_ > chipRect.top_ && rect.top_ < chipRect.bottom_)
    {
        // Xï˚å¸ï‚ê≥
        if (vel.x > 0) 
        {
            pos.x = chipRect.left_ - (rect.GetWidth() / 2.0f);
        }
        else if (vel.x < 0) 
        {
            pos.x = chipRect.right_ + (rect.GetWidth() / 2.0f);
        }

        // Yï˚å¸ï‚ê≥
        if (vel.y > 0) 
        {
            pos.y = chipRect.top_ - (rect.GetHeight() / 2.0f);
            vel.y = 0; // è∞Ç…íÖínÇµÇΩÇÁí‚é~
        }
    }
}
