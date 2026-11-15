#include "ResultScene.h"

void ResultScene::Init(Action* pAction, GameRecord* pRecord)
{
	m_pAction = pAction;
	m_pRecord = pRecord;
	m_pAction->ResetInput(); // 前のシーンで押したボタンを持ち越さない
	m_nextScene = SCENE_NONE;

	// 仮のリザルト画像(画面座標。タイトルと区別できるように黄色くする)
	m_image.Init("asset/chara_test.png", 3, 4);
	m_image.SetPos(0.0f, 0.0f, 0.0f);
	m_image.SetSize(384.0f, 384.0f, 0.0f);
	m_image.SetColor(1.0f, 1.0f, 0.3f, 1.0f);
}

void ResultScene::Update()
{
	// 決定でタイトルへ
	if (m_pAction->IsDecideTrigger()) {
		m_nextScene = SCENE_TITLE;
	}
}

void ResultScene::Draw()
{
	m_image.Draw();
}

void ResultScene::Uninit()
{
	m_image.Uninit();
}

SCENE_ID ResultScene::GetNextScene()
{
	return m_nextScene;
}
