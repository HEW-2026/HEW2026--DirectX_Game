#pragma once
//==========================================================//
// 【GameTime.h】ゲーム内の時間の進みを管理する				//
//==========================================================//
//
// Update は Game が毎フレーム呼ぶ。シーンやキャラからは呼ばない。
//
// 使い方
//   m_pTime->StartHitstop(0.15f);   // 当てた時にヒットストップを始める(秒)
//   float timeScale = m_pTime->GetScale();
//   pos.x += PER_SEC(speed) * timeScale;  // 動く量に時間の進みを掛ける(ヒットストップ中は0.1倍)
//   m_timer -= PER_SEC(1.0f) * timeScale; // 秒で数えるタイマーも同じように掛ける
//
// タイマーの数え方
//   ヒットストップで遅くしたい物(キャラの動き・技の時間など)は「秒 × timeScale」で数える
//   ヒットストップでも遅くしない物(カメラの揺れ・ヒットストップ自身)は「フレーム数」で数える
//

class GameTime
{
public:
	void Init();						// 初期化(シーンの始めに呼ぶ)
	void Update();						// 更新(Game が毎フレーム最初に呼ぶ)
	void StartHitstop(float second);	// ヒットストップを開始する(秒。ヒットストップ中にまた呼ぶと、後から来た方で上書きする)
	float GetScale();					// 今のフレームの時間の進み(倍。普段は1.0、ヒットストップ中はHITSTOP_SCALE)
	bool IsHitstop();					// 今のフレームがヒットストップ中か

private:
	int m_hitstopFrame = 0;		// ヒットストップの残りフレーム数
	bool m_isHitstop = false;	// 今のフレームがヒットストップ中か
};
