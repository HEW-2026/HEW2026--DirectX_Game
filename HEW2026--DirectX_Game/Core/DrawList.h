#pragma once
//==========================================================//
// 【DrawList.h】描く予約を集め、描画レイヤーの順に描く(Sorting Layer)
//==========================================================//
//
// Camera::Draw は、すぐに描かずにここへ予約する。Game::Draw の最後に DrawAll で、
// DRAW_LAYER の奥から順に描く(同じレイヤーの中は予約した順)。呼ぶ順番を気にしなくてよい。
// 予約した時の位置・大きさ・角度・色・numU/numV を写しておくので、1つの Object を使い回してもよい。
// DrawAll の後、Object の位置などは最後に予約した時の値になる。
//
// 使い方
//   m_camera.Draw(&m_image, x, y, w, h, DRAW_LAYER_PLAYER);   // ワールドの物(Camera が予約する)
//   m_hud.SetPos(x, y, 0.0f);                                 // UI は画面座標(中央が原点、上が+y)を Object に設定して
//   m_hud.SetSize(w, h, 0.0f);
//   g_drawList.Add(&m_hud, DRAW_LAYER_UI);                    // 直接予約する
//
// ※Object::Draw() を直接呼ぶと、DrawAll より先に描かれて一番奥になる(タイトル・リザルトは直接呼んでよい)。
//
#include "Object.h"
#include "TagLayer.h"

#define DRAW_LIST_MAX (4096) // 1フレームに予約できる数

// 予約1つ分(予約した時の Object の状態)
struct DrawItem
{
	Object* pObject;			// 描く Object
	DRAW_LAYER layer;			// 描画レイヤー
	DirectX::XMFLOAT3 pos;		// 位置(画面座標)
	DirectX::XMFLOAT3 size;		// 大きさ(ピクセル)
	float angle;				// 角度(度)
	DirectX::XMFLOAT4 color;	// 色
	float numU;					// 表示するコマ(横に何番目か)
	float numV;					// 表示するコマ(縦に何番目か)
};

class DrawList
{
public:
	void Add(Object* pObject, DRAW_LAYER layer);	// 今の Object の状態で予約する
	void DrawAll();									// レイヤーの順に全部描いて、予約を空にする(Game::Draw だけが呼ぶ)

private:
	DrawItem m_item[DRAW_LIST_MAX];	// 予約
	int m_count = 0;				// 予約の数
	bool m_isFullMessage = false;	// 「多すぎる」のメッセージを出したか
};

// ゲーム全体で1つだけ使う(Camera とシーンの両方から予約するので、グローバル変数にする)
extern DrawList g_drawList;
