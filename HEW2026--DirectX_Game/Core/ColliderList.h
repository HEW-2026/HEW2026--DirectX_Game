#pragma once
//==========================================================//
// 【ColliderList.h】1フレーム分の当たり判定の名簿(FindWithTag)
//==========================================================//
//
// 当たり判定のコピーと、持ち主の配列の何番目か(ownerIndex)を並べて持つ。毎フレーム作り直す。
// 見つけた物の持ち主は、タグ(どの配列か)と ownerIndex で分かる。例：TAG_ENEMY で ownerIndex が 3 → m_enemy[3]
//
// 使い方(全員が動いた後に作る)
//   m_colliders.Clear();
//   m_colliders.Add(m_fish.GetCollider(), 0);
//   for (int i = 0; i < ENEMY_MAX; i++) {
//       if (m_enemy[i].IsAlive()) m_colliders.Add(m_enemy[i].GetCollider(), i); // 倒した敵は入れない
//   }
//   int fishIndex = m_colliders.FindWithTag(TAG_PLAYER);        // 名簿の何番目か(無ければ -1)
//   ColliderItem fish = m_colliders.GetItem(fishIndex);         // fish.collider.GetPos() など
//   int found[COLLIDER_LIST_MAX];
//   int count = m_colliders.FindAllWithTag(TAG_ENEMY, found);   // 見つけた数。found に名簿の何番目かが入る
//
// 敵などのクラスから使う時は、Update の引数で ColliderList* を渡す。
// ※コピーなので、Add した後に動いた位置は反映されない。次のフレームで作り直す。
//
#include "Collider.h"

#define COLLIDER_LIST_MAX (256) // 1フレームに入れられる当たり判定の数

// 名簿の1件分
struct ColliderItem
{
	Collider collider;	// Add した時の当たり判定のコピー
	int ownerIndex;		// 持ち主の配列の何番目か(m_enemy[ownerIndex] など。無い時は -1)
};

class ColliderList
{
public:
	void Clear();									// 全部消す
	void Add(Collider collider, int ownerIndex);	// 足す

	int GetCount();							// 入っている数
	ColliderItem GetItem(int index);		// 名簿の index 番目(範囲外なら ownerIndex が -1 の物を返す)

	int FindWithTag(TAG tag);				// tag を持つ最初の物が名簿の何番目か(無ければ -1)
	int FindAllWithTag(TAG tag, int result[COLLIDER_LIST_MAX]);	// tag を持つ物が名簿の何番目かを全部 result に入れ、その数を返す

private:
	ColliderItem m_item[COLLIDER_LIST_MAX];	// 名簿
	int m_count = 0;						// 入っている数
	bool m_isFullMessage = false;			// 「多すぎる」のメッセージを出したか
};
