#pragma once
#include"GameObject.h"
class Input;
class Player :public GameObject
{
public:
	Player(Vector2 pos, Vector2 vel);
	~Player()override;

	virtual void Init()override;
	virtual void Update()override;
	virtual void Update(Input&input);
	virtual void Draw()override;

	void Move(Input& input);
	void Jump(Input& input);
private:
	int playerH_;//プレイヤーの画像ハンドル
	bool isJumping_;//ジャンプしているかどうか
	bool canDoubleJumping_;//だ部ジャンプ可能かどうか
};

