#pragma once
//==========================================================//
// 【DrawUtil.h】円・楕円・四角・線を描く(デバッグ表示や、簡単な演出に使う)
//==========================================================//
//
// 位置と大きさはワールド座標(ドット)。Init で渡した Camera を通して描く。
// デバッグ用(Init の2つ目が true)：F1キーで表示/非表示が切り替わる。Config.h の DEBUG_DRAW が0なら何も描かない
// 演出用  (Init の2つ目が false)：いつも描く(水流線・破片など)
//
// 使い方
//   m_draw.Init(&m_camera);         // デバッグ用
//   m_effectDraw.Init(&m_camera, false); // 演出用
//   m_draw.DrawCircle(pos.x, pos.y, radius, { 0.0f, 1.0f, 0.0f, 0.4f }); // 半透明の緑の円
//   m_draw.DrawCollider(collider, { 1.0f, 0.0f, 0.0f, 0.4f });          // 当たり判定をそのまま描く
//
#include "Object.h"
#include "Collider.h"

// Camera の前方宣言(ポインタだけ使うので、中身は .cpp で include する)
class Camera;

class DrawUtil
{
public:
	void Init(Camera* pCamera, bool isDebug = true);	//	初期化(描く時に使うカメラ、デバッグ用か)
	void Uninit();				//	終了

	void SwitchShow();	// 表示/非表示を切り替える(F1キーで呼ぶ)
	bool IsShow();		// 描くか

	//	円を描く(x, y:中心、radius:半径)
	void DrawCircle(float x, float y, float radius, DirectX::XMFLOAT4 color);
	//	楕円を描く(radiusX, radiusY:横と縦の半径、angle:向き(度))
	void DrawEllipse(float x, float y, float radiusX, float radiusY, float angle, DirectX::XMFLOAT4 color);
	//	四角を描く(x, y:中心、width, height:幅と高さ)
	void DrawBox(float x, float y, float width, float height, DirectX::XMFLOAT4 color);
	//	線を描く(x1, y1:始点、x2, y2:終点、thickness:太さ)
	void DrawLine(float x1, float y1, float x2, float y2, float thickness, DirectX::XMFLOAT4 color);
	//	当たり判定の形をそのまま描く
	void DrawCollider(Collider collider, DirectX::XMFLOAT4 color);

private:
	Object m_circle;	//	白い円の画像
	Object m_box;		//	白い四角の画像
	Camera* m_pCamera = nullptr;	// 描く時に使うカメラ
	bool m_isDebug = true;			// デバッグ用か(デバッグ用だけ F1 で消せる)
	bool m_isShow = true;			// デバッグ表示を表示中か
};
