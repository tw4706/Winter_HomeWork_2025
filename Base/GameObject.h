#pragma once
#include"Geometry.h"
#include"Rect.h"
#include"Bg.h"
#include<memory>
#include<vector>
#include<map>

class GameObject
{
protected:
	Vector2 pos_;
	Vector2 vel_;
	Rect colRect_;		//当たり判定の矩形
	bool isTurn_;		//反転してるかどうか
	bool isGround_;		//地面に接地しているかどうか
	Rect chipRect_;
	bool useGravity_;	//重力を使用するかどうか

	float graphW_;		//グラフィックの幅
	float graphH_;		//グラフィックの高さ
	float colSize_;		//当たり判定のサイズ
	Vector2 cameraOffset_;//描画オフセット
	std::shared_ptr<Bg>pBg_;//背景オブジェクトのポインタ

public:
	GameObject();
	GameObject(Vector2 pos, Vector2 vel);
	GameObject(Vector2 pos, Vector2 vel,float graphW,float graphH,float colSize);
	virtual~GameObject();

	virtual void Init()=0{};
	virtual void Update();
	virtual void Draw() = 0;

	//重力
	virtual void Gravity();

	//当たり判定の矩形を取得する関数
	const Rect& GetColRect() const { return colRect_; }

	//位置を取得する関数(ゲッター関数)
	const Vector2& GetPos()const { return pos_; }

	//Bg関連
	//Bgクラスを設定する関数
	void SetBg(std::shared_ptr<Bg>bg) { pBg_ = bg; }
	//Bgクラスを取得する関数
	std::shared_ptr<Bg>GetBg() const { return pBg_; }

	//カメラ関連
	//カメラのオフセット関連
	void SetDrawOffset(const Vector2& offset) { cameraOffset_ = offset; }
	Vector2 SetCameraOffset(const Vector2& offset) { return cameraOffset_ = offset; }

	//マップチップ関連
	//マップチップとの当たり判定
	void CheckHitMap(Rect& chipRect);
	//マップチップとの足元の判定処理
	bool IsOnGround();

	//重力の使用設定
	void SetUseGravity(bool enable);
	bool IsUsingGravity() { return useGravity_; }

};

