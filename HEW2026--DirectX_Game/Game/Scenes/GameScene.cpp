#include "GameScene.h"
#include "Config.h"
#include <math.h>

// 【サンプル】仕様書(v60)の値をドットに直したもの
#define FISH_START_X		(100.0f)			// 魚の開始位置(ドット)
#define FISH_START_Y		(384.0f)
#define ENEMY_RADIUS_X		(SPEC_PX(46.5f))	// 敵の楕円の横の半径
#define ENEMY_RADIUS_Y		(SPEC_PX(46.5f) * 0.65f)	// 敵の楕円の縦の半径
#define ROCK_RADIUS			(SPEC_PX(80.0f))	// 岩の半径
#define CAMERA_LEAD			(SPEC_PX(160.0f))	// カメラを進む向きの先へずらす量
#define ZOOM_SPEED_START	(SPEC_PX(150.0f))	// この速さまでは引かない
#define ZOOM_SPEED_FULL		(SPEC_PX(1400.0f))	// この速さで一番引く
#define ZOOM_MAX			(1.6f)				// 一番引いた時の、映す範囲の倍率
#define HITSTOP_KILL		(0.15f)				// 倒した時のヒットストップ(秒)
#define HITSTOP_DAMAGE		(0.18f)				// 被弾した時のヒットストップ(秒)
#define SHAKE_KILL			(SPEC_PX(12.0f))	// 倒した時の揺れ幅
#define SHAKE_DAMAGE		(SPEC_PX(14.0f))	// 被弾した時の揺れ幅
#define SHAKE_TIME			(0.25f)				// 揺れが収まるまでの時間(秒)
#define DAMAGE_PASS_TIME	(0.6f)				// 被弾した後、敵をすり抜ける時間(秒)
#define GOAL_MARGIN			(TILE_SIZE * 3.0f)	// ステージの右端からこれより近づいたらクリア

void GameScene::Init(Action* pAction, Sound* pSound, GameTime* pTime, GameRecord* pRecord)
{
	m_pAction = pAction;
	m_pSound = pSound;
	m_pTime = pTime;
	m_pRecord = pRecord;
	m_nextScene = SCENE_NONE;
	m_pAction->ResetInput(); // 前のシーンで押したボタンを持ち越さない

	// ステージ
	m_stage.Init("asset/Stage/sample.csv");

	// デバッグ表示
	m_draw.Init(&m_camera);

	// 【サンプル】魚・敵・岩
	m_fish.Init(FISH_START_X, FISH_START_Y);
	m_enemyImage.Init("asset/chara_test.png", 3, 4);
	m_enemyImage.SetColor(0.5f, 0.8f, 1.0f, 1.0f);
	m_rockImage.Init("asset/debug_circle.png");
	m_rockImage.SetColor(0.6f, 0.5f, 0.4f, 1.0f);
	ResetSample();
}

void GameScene::ResetSample()
{
	m_fish.Reset(FISH_START_X, FISH_START_Y);

	// 敵の位置(ワールド座標)
	DirectX::XMFLOAT2 enemyPos[SAMPLE_ENEMY_MAX] =
	{
		{ 500.0f, 384.0f },
		{ 800.0f, 300.0f },
		{ 1000.0f, 460.0f },
		{ 1500.0f, 384.0f },
		{ 2000.0f, 300.0f },
	};
	for (int i = 0; i < SAMPLE_ENEMY_MAX; i++) {
		m_enemy[i].SetEllipse(ENEMY_RADIUS_X, ENEMY_RADIUS_Y);
		m_enemy[i].SetPos(enemyPos[i].x, enemyPos[i].y);
		m_enemy[i].SetAngle(0.0f);
		m_enemyAlive[i] = true;
	}

	// 動かない岩(壁と同じで、通り抜けられない)
	m_rocks.Clear();
	m_rocks.AddCircle({ 1250.0f, 420.0f }, ROCK_RADIUS);

	m_damageTime = 0.0f;
	m_playTime = 0.0f;

	// 時間とカメラも最初に戻す(ヒットストップ・揺れ・引き具合が残らないように)
	m_pTime->Init();
	m_camera.Init();
	m_camera.SetBounds(0.0f, 0.0f, m_stage.GetWidth(), m_stage.GetHeight()); // ステージの外を映さない
	m_camera.SetPos(FISH_START_X, FISH_START_Y);
}

void GameScene::Update()
{
	// F1でデバッグ表示を切り替える
	if (m_pAction->IsDebugTrigger()) {
		m_draw.SwitchShow();
	}

	// 【サンプル】やり直し
	if (m_pAction->IsRetryTrigger()) {
		ResetSample();
	}

	float timeScale = m_pTime->GetScale();	// 時間の進み(ヒットストップ中は小さい)

	// タイマーを進める・減らす
	m_playTime += PER_SEC(1.0f) * timeScale;
	if (m_damageTime > 0.0f) {
		m_damageTime -= PER_SEC(1.0f) * timeScale;
	}

	// 【サンプル】魚を動かす(タイルの壁と岩にぶつかったら滑る)
	m_fish.Update(m_pAction, &m_camera, m_stage.GetWalls(), &m_rocks, timeScale);

	// 【サンプル】当たり判定
	CheckHit();

	// カメラを魚に合わせる
	UpdateCamera();
	m_camera.Update();

	// 【サンプル】ステージの右端まで来たらクリア(ステージが読み込めていない時は調べない)
	if (m_stage.GetWidth() > 0.0f && m_fish.GetPos().x > m_stage.GetWidth() - GOAL_MARGIN) {
		m_pRecord->clearTime[m_pRecord->stageNo] = m_playTime;
		m_nextScene = SCENE_RESULT;
	}
}

void GameScene::CheckHit()
{
	Collider fish = m_fish.GetCollider();
	for (int i = 0; i < SAMPLE_ENEMY_MAX; i++) {
		if (!m_enemyAlive[i]) continue;
		if (!IsHit(fish, m_enemy[i])) continue;

		// 回転中か、正面から当たったら倒す
		if (m_fish.IsSpinning() || IsFront(m_fish.GetPos(), m_fish.GetAngle(), m_enemy[i])) {
			m_enemyAlive[i] = false;
			m_pTime->StartHitstop(HITSTOP_KILL);
			m_camera.Shake(SHAKE_KILL, SHAKE_TIME);
			m_pSound->Play(SOUND_LABEL_SE000);
		}
		// 横や後ろから当たったら被弾(被弾した後しばらくは、すり抜ける)
		else if (m_damageTime <= 0.0f) {
			m_damageTime = DAMAGE_PASS_TIME;
			m_fish.StopSpin();
			m_pTime->StartHitstop(HITSTOP_DAMAGE);
			m_camera.Shake(SHAKE_DAMAGE, SHAKE_TIME);
		}
	}
}

void GameScene::UpdateCamera()
{
	// 進む向きの少し先を映す(先が見えるように)
	DirectX::XMFLOAT2 pos = m_fish.GetPos();
	float rad = m_fish.GetAngle() * DEG_TO_RAD;
	m_camera.SetTarget(pos.x + cosf(rad) * CAMERA_LEAD, pos.y + sinf(rad) * CAMERA_LEAD);

	// 速いほど引いて、広い範囲を映す(ZOOM_SPEED_START 〜 ZOOM_SPEED_FULL の間は速さに比例)
	float rate = (m_fish.GetSpeed() - ZOOM_SPEED_START) / (ZOOM_SPEED_FULL - ZOOM_SPEED_START);
	if (rate < 0.0f) rate = 0.0f;
	if (rate > 1.0f) rate = 1.0f;
	m_camera.SetViewScale(1.0f + (ZOOM_MAX - 1.0f) * rate);
}

void GameScene::Draw()
{
	// ステージ
	m_stage.Draw(&m_camera);

	// 【サンプル】岩・敵・魚
	for (int i = 0; i < m_rocks.GetCircleCount(); i++) {
		WallCircle rock = m_rocks.GetCircle(i);
		m_camera.Draw(&m_rockImage, rock.center.x, rock.center.y, rock.radius * 2.0f, rock.radius * 2.0f);
	}
	for (int i = 0; i < SAMPLE_ENEMY_MAX; i++) {
		if (!m_enemyAlive[i]) continue;
		DirectX::XMFLOAT2 pos = m_enemy[i].GetPos();
		m_camera.Draw(&m_enemyImage, pos.x, pos.y, ENEMY_RADIUS_X * 2.0f, ENEMY_RADIUS_X * 2.0f);
	}
	m_fish.Draw(&m_camera);

	// デバッグ表示(F1で切り替え)
	m_stage.GetWalls()->DrawDebug(&m_draw);
	m_rocks.DrawDebug(&m_draw);
	for (int i = 0; i < SAMPLE_ENEMY_MAX; i++) {
		if (!m_enemyAlive[i]) continue;
		m_draw.DrawCollider(m_enemy[i], { 1.0f, 1.0f, 1.0f, 0.3f });
	}
	// 魚の当たり判定(被弾した後は赤、回転中は黄色、普段は緑)
	DirectX::XMFLOAT4 color = { 0.0f, 1.0f, 0.0f, 0.4f };
	if (m_fish.IsSpinning()) color = { 1.0f, 1.0f, 0.0f, 0.5f };
	if (m_damageTime > 0.0f) color = { 1.0f, 0.0f, 0.0f, 0.5f };
	m_fish.DrawDebug(&m_draw, color);
}

void GameScene::Uninit()
{
	m_fish.Uninit();
	m_enemyImage.Uninit();
	m_rockImage.Uninit();
	m_draw.Uninit();
	m_stage.Uninit();
}

SCENE_ID GameScene::GetNextScene()
{
	return m_nextScene;
}
