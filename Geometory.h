#pragma once
#include<cmath>
class Geometory
{
public:
	float x;
	float y;

public:
	Geometory() :
		x(0.0f),
		y(0.0f)
	{
	}
	Geometory(float posX, float posY) :
		x(posX),
		y(posY)
	{
	}
	//’P€‰‰ŽZŽq+ Vec2=+Vec2
	Geometory operator+()const
	{
		return *this;
	}
	//’P€‰‰ŽZŽq- Vec2=-Vec2
	Geometory operator-()const
	{
		return Geometory{ -x, -y };
	}
	//ƒxƒNƒgƒ‹‚Ì‘«‚µŽZ
	Geometory operator+(Geometory geo)const
	{
		return Geometory{ x + geo.x, y+geo.y };
	}
	//Vec2+=Vec2
	Geometory operator+=(Geometory geo)
	{
		x += geo.x;
		y += geo.y;
		return *this;
	}

	//ˆø‚«ŽZ
	Geometory operator-(Geometory geo)const
	{
		return Geometory{ x - geo.x, y - geo.y };
	}
	//Vec2-=Vec2
	Geometory operator-=(Geometory geo)
	{
		x -= geo.x;
		y -= geo.y;
		return *this;
	}

	//Š|‚¯ŽZ
	Geometory operator*(float scale)const
	{
		return Geometory{ x *scale, y *scale };
	}
	//Vec2*=float
	Geometory operator*=(float scale)
	{
		x *= scale;
		y *= scale;
		return *this;
	}

	//Š„‚èŽZ
	Geometory operator/(float scale)const
	{
		return Geometory{ x /scale, y / scale };
	}
	//Vec2+=Vec2
	Geometory operator/=(float scale)
	{
		x /= scale;
		y /= scale;
		return *this;
	}
	
	//ƒxƒNƒgƒ‹‚Ì’·‚³‚ð‹‚ß‚é
	float Length()const
	{
		return sqrtf(x * x + y * y);
	}
	//ƒxƒNƒgƒ‹‚Ì³‹K‰»
	float Normalize()
	{
		float len = Length();
		if (len > 0.0f)
		{
			x /= len;
			y /= len;
			return len;
		}
		else if (len == 0)
		{
			return 0.0f;
		}
	}
};

