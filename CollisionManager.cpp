#include "CollisionManager.h"
#include<cmath>

CollisionManager::CollisionManager()
{
}

CollisionManager::~CollisionManager()
{
}

void CollisionManager::ResolveCollision(Rect& rect, Vector2& pos,Vector2& vel, const Rect& chipRect) 
{
    // プレイヤーの矩形を更新
    rect.left_ = pos.x;
    rect.top_ = pos.y;
    rect.right_ = pos.x + rect.GetWidth();
    rect.bottom_ = pos.y + rect.GetHeight();

    // 衝突判定
    if (rect.right_ > chipRect.left_ && rect.left_ < chipRect.right_ &&
        rect.bottom_ > chipRect.top_ && rect.top_ < chipRect.bottom_)
    {
        // 移動前の座標
        float oldX = pos.x - vel.x;
        float oldY = pos.y - vel.y;

        // 衝突方向判定と補正
        // 左から衝突
        if (oldX + rect.GetWidth() <= chipRect.left_) {
            pos.x = chipRect.left_ - rect.GetWidth();
            vel.x = 0;
        }
        // 右から衝突
        else if (oldX >= chipRect.right_) {
            pos.x = chipRect.right_;
            vel.x = 0;
        }
        // 上から衝突
        else if (oldY + rect.GetHeight() <= chipRect.top_) {
            pos.y = chipRect.top_ - rect.GetHeight();
            vel.y = 0;
        }
        // 下から衝突
        else if (oldY >= chipRect.bottom_) {
            pos.y = chipRect.bottom_;
            vel.y = 0;
        }
    }
}
