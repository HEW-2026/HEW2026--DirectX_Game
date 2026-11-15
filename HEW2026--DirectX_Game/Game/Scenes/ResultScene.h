#pragma once
//==========================================================//
// 【ResultScene.h】リザルト(ALL CLEAR)。決定でタイトルへ戻る
//==========================================================//
// 今は仮の画面。各ステージのタイムの表示は、数字の画像ができてから足す。
//
#include "Object.h"
#include "Action.h"
#include "SceneID.h"
#include "GameRecord.h"

class ResultScene
{
public:
	void Init(Action* pAction, GameRecord* pRecord);	// 初期化(pRecord:各ステージのタイム)
	void Update();				// 更新
	void Draw();				// 描画
	void Uninit();				// 終了
	SCENE_ID GetNextScene();	// 次のシーン(変えない時はSCENE_NONE)

private:
	Action* m_pAction = nullptr;		// 操作
	GameRecord* m_pRecord = nullptr;	// 各ステージのタイム(数字の画像ができたら表示する)
	Object m_image;						// 仮のリザルト画像
	SCENE_ID m_nextScene = SCENE_NONE;	// 次のシーン
};
