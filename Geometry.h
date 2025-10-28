#pragma once
//2Dベクトルクラス
struct Vector2
{
	float x, y;
	float Length()const;//ベクトルの大きさ
	void Normalize();//大きさを1に
	Vector2 Normalized()const;//大きさを1にしたベクトルを返す
	Vector2 operator-()const;//ベクトルを反転したベクトルを返す
	void operator+=(const Vector2& val);//加算
	void operator-=(const Vector2& val);//減算
	void operator*=(const float scale);//ベクトルをn倍
	Vector2 operator+(const Vector2& val)const;//ベクトルの加算
	Vector2 operator-(const Vector2& val)const;//ベクトルの減算
	Vector2 operator*(float scale)const;//ベクトルの乗算
};

//2Dのサイズを定義する構造体
struct Size {
	int w;//幅
	int h;//高さ
};