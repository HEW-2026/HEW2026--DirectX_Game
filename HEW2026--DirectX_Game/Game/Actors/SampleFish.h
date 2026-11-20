#pragma once
//==========================================================//
// 【SampleFish.h】Core の使い方を見せるための、簡単な魚(サンプル)
//==========================================================//
//
// 本番の Player はこれを参考に作る(仕様書 v60 の加速・ドリフトなどはまだ入っていない)。
// 使っている Core：Body(移動と壁)、Collider(当たり判定)、Camera(描画・カーソル)、Action(操作)
//
#include "Object.h"
#include "Body.h"
#include "Action.h"
#include "Camera.h"
#include "DrawUtil.h"

class SampleFish
{
public:
	void Init(float x, float y);	// 初期化(位置はワールド座標)
	// 更新(timeScale：時間の進み。GameTime::GetScale() を渡す)
	// pWalls:タイルの壁、pRocks:岩などの動かない円
	void Update(Action* pAction, Camera* pCamera, WallList* pWalls, WallList* pRocks, float timeScale);
	void Draw(Camera* pCamera);		// 描画
	void DrawDebug(DrawUtil* pDraw, DirectX::XMFLOAT4 color);	// 当たり判定を描く
	void Uninit();					// 終了

	void Reset(float x, float y);	// 位置と状態を最初に戻す

	Collider GetCollider();		// 当たり判定(TAG_PLAYER、HIT_LAYER_PLAYER)
	DirectX::XMFLOAT2 GetPos();	// 位置
	float GetAngle();			// 頭の向き(度)
	float GetSpeed();			// 今の速さ(ドット/秒。ドリフト中の減速は含まない)
	bool IsSpinning();			// 回転アタック中か
	void StopSpin();			// 回転アタックをやめる

private:
	Body m_body;			// 体(位置・半径・壁との当たり)
	Object m_image;			// 画像
	float m_angle = 0.0f;	// 頭の向き(度。0で右、90で上)
	float m_spinTime = 0.0f;	// 回転アタックの残り時間(秒)
	float m_dashTime = 0.0f;	// ドリフトを離した後の加速の残り時間(秒)
	int m_animFrame = 0;		// アニメーション用のフレーム数

	void UpdateAngle(Action* pAction, Camera* pCamera, float timeScale);	// 向きを変える
};
