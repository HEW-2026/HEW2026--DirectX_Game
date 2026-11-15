#pragma once
//==========================================================//
// 【GameScene.h】ゲーム本体のシーン
//==========================================================//
//
// 今は Core の使い方を見せるサンプルが入っている(【サンプル】と書いた所)。
// 本番では、サンプルの魚と敵を Player・EnemyManager などのクラスに置き換える。
//
// 1フレームの順番
//   Update：タイマー → 魚(壁・岩に当たって滑る) → 当たり判定(CheckHit) → カメラ
//   Draw  ：ステージ → 岩・敵 → 魚 → デバッグ表示
//
// Init は2回目以降(もう一度遊ぶ時)も呼ばれるので、メンバーは全部 Init か ResetSample で決め直すこと。
//
#include "Object.h"
#include "Action.h"
#include "sound.h"
#include "GameTime.h"
#include "Camera.h"
#include "DrawUtil.h"
#include "Collider.h"
#include "WallList.h"
#include "Stage.h"
#include "SceneID.h"
#include "GameRecord.h"
#include "SampleFish.h"

#define SAMPLE_ENEMY_MAX (5) // 【サンプル】敵の数

class GameScene
{
public:
	void Init(Action* pAction, Sound* pSound, GameTime* pTime, GameRecord* pRecord);	// 初期化
	void Update();				// 更新
	void Draw();				// 描画
	void Uninit();				// 終了
	SCENE_ID GetNextScene();	// 次のシーン(変えない時はSCENE_NONE)

private:
	Action* m_pAction = nullptr;	// 操作
	Sound* m_pSound = nullptr;		// サウンド
	GameTime* m_pTime = nullptr;	// ゲーム内の時間
	GameRecord* m_pRecord = nullptr;	// 記録(クリアタイムを書き込む)
	SCENE_ID m_nextScene = SCENE_NONE;	// 次のシーン

	Stage m_stage;		// ステージ
	Camera m_camera;	// カメラ
	DrawUtil m_draw;	// デバッグ表示
	float m_playTime = 0.0f;	// 経過時間(秒。ヒットストップ中はゆっくり進む)

	// 【サンプル】ここから
	SampleFish m_fish;							// 魚
	Object m_enemyImage;						// 敵の画像(全部の敵で1つを使い回す)
	Object m_rockImage;							// 岩の画像
	Collider m_enemy[SAMPLE_ENEMY_MAX];			// 敵の当たり判定
	bool m_enemyAlive[SAMPLE_ENEMY_MAX] = {};	// 敵が生きているか
	WallList m_rocks;							// 動かない岩(壁と同じで、通り抜けられない)
	float m_damageTime = 0.0f;	// 被弾した後、敵をすり抜ける残り時間(秒)

	void ResetSample();	// 魚・敵・カメラ・時間を最初の状態に戻す
	void CheckHit();	// 魚と敵の当たり判定
	void UpdateCamera();	// カメラを魚に合わせる
	// 【サンプル】ここまで
};
