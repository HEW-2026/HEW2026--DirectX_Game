#pragma once
//==========================================================//
// 【TagLayer.h】タグ・当たり判定レイヤー・描画レイヤーの一覧
//==========================================================//
//
// Unity の Tags & Layers の設定と同じ役割。種類を増やす時はこのファイルに足す。
// HIT_LAYER を増やした時は、TagLayer.cpp の表にも行と列を足す。
//
//   TAG        ：物の種類(Unity の Tag)
//                collider.SetTag(TAG_ENEMY);  if (collider.CompareTag(TAG_ENEMY)) { ... }
//   HIT_LAYER  ：当たる組み合わせ(Unity の Layer)
//                collider.SetHitLayer(HIT_LAYER_ENEMY);
//   DRAW_LAYER ：描く順番(Unity の Sorting Layer)
//                m_camera.Draw(&m_image, x, y, w, h, DRAW_LAYER_PLAYER);
//

// 物の種類
enum TAG
{
	TAG_NONE,	// 未設定
	TAG_PLAYER,	// 魚
	TAG_ENEMY,	// 敵
	TAG_MAX,	// 種類の数(一番最後に置く)
};

// 当たり判定のレイヤー(どれとどれが当たるかは TagLayer.cpp の表で決める)
enum HIT_LAYER
{
	HIT_LAYER_DEFAULT,	// 未設定(全部と当たる)
	HIT_LAYER_PLAYER,	// 魚
	HIT_LAYER_ENEMY,	// 敵
	HIT_LAYER_MAX,		// レイヤーの数(一番最後に置く。増やしたら TagLayer.cpp の表に行と列を足す)
};

// 描画のレイヤー(先に書いたものほど奥。同じレイヤーの中は予約した順)
enum DRAW_LAYER
{
	DRAW_LAYER_BACKGROUND,		// 背景
	DRAW_LAYER_STAGE,			// タイル
	DRAW_LAYER_CHARACTER,		// 敵・岩・ボス
	DRAW_LAYER_EFFECT_BACK,		// 魚の後ろの演出(泡・残像など)
	DRAW_LAYER_PLAYER,			// 魚
	DRAW_LAYER_EFFECT_FRONT,	// 魚の前の演出(ヒットの破片など)
	DRAW_LAYER_UI,				// HUD(画面座標)
	DRAW_LAYER_DEBUG,			// デバッグ表示(一番手前)
	DRAW_LAYER_MAX,				// レイヤーの数(一番最後に置く)
};

// 2つのレイヤーが当たる組み合わせか
bool IsLayerHit(HIT_LAYER a, HIT_LAYER b);
