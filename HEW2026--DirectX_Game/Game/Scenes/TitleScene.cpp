#include "TitleScene.h"

void TitleScene::Init(Action* pAction)
{
	m_pAction = pAction;
	m_pAction->ResetInput(); // 前のシーンで押したボタンを持ち越さない
	m_frame = 0;
	m_nextScene = SCENE_NONE;

	// 仮のタイトル画像(画面座標。画面中央に大きく表示する)
	m_image.Init("asset/chara_test.png", 3, 4);
	m_image.SetPos(0.0f, 0.0f, 0.0f);
	m_image.SetSize(384.0f, 384.0f, 0.0f);
}

void TitleScene::Update()
{
	m_frame++;

	// 決定でゲームへ
	if (m_pAction->IsDecideTrigger()) {
		m_nextScene = SCENE_GAME;
	}
}

void TitleScene::Draw()
{
	// 「押してください」の代わりに、30フレームごとに明るさを変える(仮)
	if ((m_frame / 30) % 2 == 0) {
		m_image.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else {
		m_image.SetColor(0.6f, 0.6f, 0.6f, 1.0f);
	}
	m_image.Draw();
}

void TitleScene::Uninit()
{
	m_image.Uninit();
}

SCENE_ID TitleScene::GetNextScene()
{
	return m_nextScene;
}
