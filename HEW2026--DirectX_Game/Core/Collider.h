#pragma once
//==========================================================//
// 【Collider.h】当たり判定の形(円・楕円)と、当たったかの判定
//==========================================================//
//
// 位置はワールド座標(ドット)、向きは度。
// 壁との当たりは Body が行う。Collider は「キャラ同士が重なったか」を調べるのに使う。
//
// 使い方
//   m_collider.SetEllipse(35.0f, 23.0f);  // 楕円にする(横の半径、縦の半径)
//   m_collider.SetPos(x, y);              // 毎フレーム、キャラの位置に合わせる
//   m_collider.SetTag(TAG_ENEMY);            // 物の種類(TagLayer.h)
//   m_collider.SetHitLayer(HIT_LAYER_ENEMY); // 当たる組み合わせ(TagLayer.cpp の表)
//   if (IsHit(fish.GetCollider(), m_collider)) { ... }              // 重なったか(当たらないレイヤー同士は false)
//   if (other.CompareTag(TAG_ENEMY)) { ... }                        // 相手が敵か
//   if (IsFront(fishPos, fishAngle, m_collider)) { ... }            // 魚の正面に当たったか
//
// ※楕円と楕円の判定は未対応(いつも false になる)。今のゲームでは円と楕円しか使わない。
//
#include <DirectXMath.h>
#include "TagLayer.h"

//	当たり判定の形の種類
enum COLLIDER_TYPE
{
	COLLIDER_CIRCLE,	//	円
	COLLIDER_ELLIPSE,	//	楕円
};

//	当たり判定の形・大きさ・位置をまとめて持つ
class Collider
{
public:
	void SetCircle(float radius);	//	円にする(radius:半径)
	void SetEllipse(float radiusX, float radiusY);	//	楕円にする(radiusX:横の半径、radiusY:縦の半径)
	void SetPos(float x, float y);	//	中心の位置
	void SetAngle(float angle);	//	向き(度)。楕円を回す時に使う
	void SetTag(TAG tag);	//	物の種類
	void SetHitLayer(HIT_LAYER layer);	//	当たり判定のレイヤー

	COLLIDER_TYPE GetType();	//	形の種類
	DirectX::XMFLOAT2 GetPos();	//	中心の位置
	float GetRadiusX();	//	横の半径(円の時は半径)
	float GetRadiusY();	//	縦の半径(円の時は半径)
	float GetAngle();	//	向き(度)
	TAG GetTag();	//	物の種類
	bool CompareTag(TAG tag);	//	物の種類が tag か
	HIT_LAYER GetHitLayer();	//	当たり判定のレイヤー

	//	縁の上で、中心から見て point の方向にある点(当たった場所を求める時に使う)
	DirectX::XMFLOAT2 GetEdgePoint(DirectX::XMFLOAT2 point);

private:
	COLLIDER_TYPE m_type = COLLIDER_CIRCLE;	//	形の種類
	DirectX::XMFLOAT2 m_pos = { 0.0f,0.0f };	//	中心の位置
	float m_radiusX = 0.0f;	//	横の半径（円の時は半径）
	float m_radiusY = 0.0f;	//	縦の半径（円の時は半径と同じ）
	float m_angle = 0.0f;	//	向き（度）
	TAG m_tag = TAG_NONE;	//	物の種類
	HIT_LAYER m_hitLayer = HIT_LAYER_DEFAULT;	//	当たり判定のレイヤー
};

//	2つの当たり判定が重なっているか(当たらないレイヤー同士と、楕円と楕円はいつも false)
bool IsHit(Collider a, Collider b);

//	targetが自分の正面(向きの左右90度以内)にあるか(myPos:自分の中心、angle:自分の向き(度))
//	相手の中心ではなく縁の点で調べる(大きい相手でも当たった場所で判定)
bool IsFront(DirectX::XMFLOAT2 myPos, float angle, Collider target);
