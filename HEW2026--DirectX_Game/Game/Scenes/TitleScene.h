#pragma once
//==========================================================//
// 【TitleScene.h】タイトル(説明画面)。決定でゲームへ進む
//==========================================================//
// 今は仮の画面(画像を1枚表示するだけ)。説明画面の画像ができたら差し替える。
//
#include "Object.h"
#include "Action.h"
#include "SceneID.h"

class TitleScene
{
public:
	void Init(Action* pAction);	// 初期化
	void Update();				// 更新
	void Draw();				// 描画
	void Uninit();				// 終了
	SCENE_ID GetNextScene();	// 次のシーン(変えない時はSCENE_NONE)

private:
	Action* m_pAction = nullptr;		// 操作
	Object m_image;						// 仮のタイトル画像
	int m_frame = 0;					// 経過フレーム数(点滅に使う)
	SCENE_ID m_nextScene = SCENE_NONE;	// 次のシーン
};
