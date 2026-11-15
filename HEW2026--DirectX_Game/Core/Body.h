#pragma once
//==========================================================//
// 【Body.h】円の体を動かし、壁にぶつかったら押し出して滑らせる
//==========================================================//
//
// 位置はワールド座標(ドット。ステージの左下が原点、右が+x、上が+y)。
//
// 使い方
//   m_body.Init(100.0f, 200.0f, SPEC_PX(16.0f));      // 位置と半径
//   // 1フレームに動く量を渡す。秒単位の速さは PER_SEC() で直し、時間の進みを掛ける
//   DirectX::XMFLOAT2 move = { PER_SEC(speedX) * timeScale, PER_SEC(speedY) * timeScale };
//   m_body.Move(move, m_stage.GetWalls(), &m_rocks);   // タイルの壁と岩(円)にぶつかって滑る
//   Collider c = m_body.GetCollider();                  // 敵との当たり判定に使う円
//
// 仕組み：移動を細かい歩に分け、1歩ごとに壁から直角に押し出す(これで壁に沿って滑る)。
//         それでもめり込んだら1歩戻して止まる。
//
#include <DirectXMath.h>
#include "Collider.h"
#include "WallList.h"

#define BODY_STEP_MAX	(4.0f)	// 1歩の最大の長さ(ドット)。半径の半分の方が小さければそちらを使う
#define BODY_STEP_LIMIT	(64)	// 1フレームの歩数の上限(超える移動はしない)

class Body
{
public:
	void Init(float x, float y, float radius);	// 初期化(位置と半径)

	// 動かす(move:1フレームで動く量(ドット)、pWalls・pWalls2:ぶつかる壁。要らなければnullptr)
	// 壁に当たったら true を返す
	bool Move(DirectX::XMFLOAT2 move, WallList* pWalls, WallList* pWalls2 = nullptr);

	void SetPos(float x, float y);		// 位置をすぐに変える(壁は調べない。壁や岩に重なる所に置かないこと)
	DirectX::XMFLOAT2 GetPos();			// 位置
	float GetRadius();					// 半径
	Collider GetCollider();				// 当たり判定用の円

private:
	DirectX::XMFLOAT2 m_pos = { 0.0f, 0.0f };	// 中心の位置(ドット)
	float m_radius = 0.0f;						// 半径(ドット)

	bool PushOutOfWalls(WallList* pWalls);	// 近すぎる壁から1回押し出す。押し出したらtrue
	bool IsInWall(WallList* pWalls);		// 壁にめり込んでいるか
};
