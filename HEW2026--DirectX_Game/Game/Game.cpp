#include "Game.h"

void Game::Init(HWND hWnd)
{
	RendererInit(hWnd); // DirectXを初期化

	sound.Init();                   // サウンドを初期化
	sound.Play(SOUND_LABEL_BGM000); // BGMを再生

	m_time.Init();               // ゲーム内の時間を初期化
	m_action.Init(hWnd, &input); // 操作を初期化

	m_record = {};               // 記録を空にする(全部0)

	ChangeScene(SCENE_TITLE);    // タイトルから始める
}

void Game::Update(void)
{
	input.Update();    // キー入力を更新
	m_time.Update();   // ゲーム内の時間を更新
	m_action.Update(); // 操作を更新

	// 今のシーンを更新し、次のシーンを聞く
	SCENE_ID next = SCENE_NONE;
	switch (m_scene) {
	case SCENE_TITLE:
		m_titleScene.Update();
		next = m_titleScene.GetNextScene();
		break;
	case SCENE_GAME:
		m_gameScene.Update();
		next = m_gameScene.GetNextScene();
		break;
	case SCENE_RESULT:
		m_resultScene.Update();
		next = m_resultScene.GetNextScene();
		break;
	}

	// 次のシーンが決まったら切り替える
	if (next != SCENE_NONE) {
		ChangeScene(next);
	}
}

void Game::Draw(void)
{
	RendererDrawStart(); // 描画開始

	switch (m_scene) {
	case SCENE_TITLE:	m_titleScene.Draw();	break;
	case SCENE_GAME:	m_gameScene.Draw();		break;
	case SCENE_RESULT:	m_resultScene.Draw();	break;
	}

	RendererDrawEnd();   // 描画終了
}

void Game::Uninit(void)
{
	ChangeScene(SCENE_NONE); // 今のシーンを終了

	sound.Uninit();    // サウンドを終了
	RendererUninit();  // DirectXを終了
}

void Game::ChangeScene(SCENE_ID next)
{
	// 今のシーンを終了する
	switch (m_scene) {
	case SCENE_TITLE:	m_titleScene.Uninit();	break;
	case SCENE_GAME:	m_gameScene.Uninit();	break;
	case SCENE_RESULT:	m_resultScene.Uninit();	break;
	}

	// 次のシーンを初期化する(画像の読み込みもここで行われる)
	m_scene = next;
	switch (m_scene) {
	case SCENE_TITLE:	m_titleScene.Init(&m_action);						break;
	case SCENE_GAME:	m_gameScene.Init(&m_action, &sound, &m_time, &m_record);	break;
	case SCENE_RESULT:	m_resultScene.Init(&m_action, &m_record);				break;
	}
}
