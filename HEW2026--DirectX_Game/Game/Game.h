#pragma once
//==========================================================//
// 【Game.h】ゲーム全体。共通の物(入力・音・時間・操作・記録)を持ち、シーンを切り替える
//==========================================================//
//
// シーンの切り替え方
//   各シーンの GetNextScene() が SCENE_NONE 以外を返したら、
//   今のシーンを Uninit して、次のシーンを Init する。
// 新しいシーンを足す時
//   1. SceneID.h に SCENE_〜 を足す
//   2. ここにシーンのメンバーを足し、Game.cpp の4つの switch に case を足す
//
#include "Object.h"
#include "input.h"
#include "sound.h"
#include "GameTime.h"
#include "Action.h"
#include "SceneID.h"
#include "GameRecord.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"

class Game {
private:
	Input input; //キー入力
	Sound sound; //サウンド

	GameTime m_time;	//ゲーム内の時間
	Action m_action;	//操作
	GameRecord m_record = {};	//シーンをまたいで残す記録

	SCENE_ID m_scene = SCENE_NONE;	//今のシーン
	TitleScene m_titleScene;		//タイトル
	GameScene m_gameScene;			//ゲーム
	ResultScene m_resultScene;		//リザルト

	void ChangeScene(SCENE_ID next);	//シーンを切り替える

public:
	void Init(HWND hWnd); // 初期化
	void Update();        // 更新
	void Draw();          // 描画
	void Uninit();        // 終了
};
