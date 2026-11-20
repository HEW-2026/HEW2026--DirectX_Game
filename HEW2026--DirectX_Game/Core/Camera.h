#pragma once
//==========================================================//
// 【Camera.h】ワールド(ステージ)のどこを画面に映すかを管理する
//==========================================================//
//
// 座標は2種類ある
//   ワールド座標：ステージの中の位置(ドット)。左下が原点、右が+x、上が+y
//   画面座標    ：Object に渡す位置(画面の中央が原点、右が+x、上が+y、1920×1080)
// ゲームの処理は全部ワールド座標で書き、描く時だけ Camera が画面座標に直す。
// Draw はすぐには描かず、DrawList に予約する(実際に描くのは Game::Draw の最後。DrawList.h)。
//
// 使い方
//   m_camera.SetTarget(pos.x, pos.y);      // 追いかける位置(毎フレーム)
//   m_camera.SetViewScale(1.3f);           // 引く(1.3倍の範囲を映す)
//   m_camera.Shake(12.0f, 0.25f);          // 揺らす(強さ(ドット)、時間(秒))
//   m_camera.Update();                     // 毎フレーム1回(カメラを持っているシーンが呼ぶ)
//   m_camera.Draw(&m_image, x, y, w, h, DRAW_LAYER_PLAYER); // Object をワールド座標で描く(描画レイヤーを省くと DRAW_LAYER_CHARACTER)
//   DirectX::XMFLOAT2 world = m_camera.ScreenToWorld(m_pAction->GetMousePos()); // カーソルの位置をワールド座標に
//
#include <DirectXMath.h>
#include "Object.h"
#include "TagLayer.h"

#define CAMERA_FOLLOW_RATE		(6.0f)	// 追いつく速さ(1フレームで残りの距離の 6/60 近づく)
#define CAMERA_ZOOM_OUT_RATE	(3.0f)	// 引く時の速さ(考え方は CAMERA_FOLLOW_RATE と同じ)
#define CAMERA_ZOOM_IN_RATE		(1.2f)	// 戻る時の速さ(引く時より遅くし、ズームの細かい往復を防ぐ)

class Camera
{
public:
	void Init();	// 初期化
	void Update();	// 更新(毎フレーム1回。ヒットストップ中も普通に動かす)

	void SetTarget(float x, float y);	// 追いかける位置(ワールド座標)
	void SetPos(float x, float y);		// すぐにその位置へ移す(ステージ開始時など)
	void SetFollowRate(float rate);		// 追いつく速さを変える(普段は CAMERA_FOLLOW_RATE)
	void SetViewScale(float scale);		// 映す範囲の倍率の目標(1.0:普通、1.6:1.6倍の範囲を映す)
	void Shake(float power, float second);	// 揺らす(power:揺れ幅(ドット)、second:収まるまでの時間(秒))。揺れている時に呼ぶと上書きする
	void SetBounds(float left, float bottom, float right, float top); // この範囲の外を映さない(ステージの大きさ)

	// 座標の変換
	DirectX::XMFLOAT2 WorldToScreen(DirectX::XMFLOAT2 world);	// ワールド座標 → 画面座標
	DirectX::XMFLOAT2 ScreenToWorld(DirectX::XMFLOAT2 screen);	// 画面座標 → ワールド座標
	float WorldToScreenLength(float length);					// ワールドの長さ → 画面の長さ

	// Object をワールド座標で描く予約をする(x, y:中心、width, height:大きさ(ドット)、layer:描画レイヤー)
	// 角度・色・numU/numV は Object に先に設定しておく。画面の外の物は予約しない
	void Draw(Object* pObject, float x, float y, float width, float height, DRAW_LAYER layer = DRAW_LAYER_CHARACTER);

	// 今映している範囲(ワールド座標)。Draw は画面外の物を自動で予約しない
	float GetViewLeft();
	float GetViewRight();
	float GetViewBottom();
	float GetViewTop();

private:
	DirectX::XMFLOAT2 m_pos = { 0.0f, 0.0f };		// 画面の中央に映す位置(ワールド座標)
	DirectX::XMFLOAT2 m_target = { 0.0f, 0.0f };	// 追いかける位置
	DirectX::XMFLOAT2 m_shakeOffset = { 0.0f, 0.0f };	// 揺れでずれている量
	float m_viewScale = 1.0f;		// 今の映す範囲の倍率
	float m_followRate = CAMERA_FOLLOW_RATE;	// 追いつく速さ
	float m_viewScaleTarget = 1.0f;	// 目標の映す範囲の倍率
	float m_shakePower = 0.0f;		// 揺れ幅(ドット)
	int m_shakeFrame = 0;			// 揺れの残りフレーム数
	int m_shakeFrameMax = 0;		// 揺れ始めた時のフレーム数
	bool m_useBounds = false;		// 範囲の制限を使うか
	float m_boundLeft = 0.0f;		// 範囲の左端
	float m_boundBottom = 0.0f;		// 範囲の下端
	float m_boundRight = 0.0f;		// 範囲の右端
	float m_boundTop = 0.0f;		// 範囲の上端

	float GetScreenScale();		// ワールドの1ドットが画面で何ピクセルか
	void KeepInBounds();		// 範囲の外を映さないように位置を直す
	bool IsInView(float x, float y, float radius);	// 中心 x, y・半径 radius の円が、画面に少しでも映るか
};
