#include "SampleFish.h"
#include "Config.h"
#include <math.h>

#define FISH_RADIUS		(SPEC_PX(16.0f))	// 当たり判定の半径(ドット)
#define FISH_SIZE		(SPEC_PX(48.0f))	// 画像の大きさ(ドット)
#define FISH_SPEED		(SPEC_PX(150.0f))	// 普段の速さ(ドット/秒)
#define FISH_TURN_SPEED	(240.0f)			// 向きを変える速さ(度/秒)
#define MOUSE_DEADZONE	(SPEC_PX(40.0f))	// カーソルがこれより近い時は向きを変えない(ドット)
#define SPIN_TIME		(0.5f)				// 回転アタックの時間(秒)
#define DRIFT_SLOW		(0.3f)				// ドリフト中の速さ(倍)
#define DASH_TIME		(0.5f)				// ドリフトを離した後に速くなる時間(秒)
#define DASH_SPEED		(3.0f)				// ドリフトを離した後の速さ(倍)

void SampleFish::Init(float x, float y)
{
	m_image.Init("asset/chara_test.png", 3, 4);
	Reset(x, y);
}

void SampleFish::Reset(float x, float y)
{
	m_body.Init(x, y, FISH_RADIUS);
	m_angle = 0.0f;
	m_spinTime = 0.0f;
	m_dashTime = 0.0f;
	m_animFrame = 0;
}

void SampleFish::Update(Action* pAction, Camera* pCamera, WallList* pWalls, WallList* pRocks, float timeScale)
{
	// 向きを変える
	UpdateAngle(pAction, pCamera, timeScale);

	// 回転アタックを始める
	if (pAction->IsSpinTrigger() && m_spinTime <= 0.0f) {
		m_spinTime = SPIN_TIME;
	}
	// ドリフトを離したら、しばらく速くなる
	if (pAction->IsDriftRelease()) {
		m_dashTime = DASH_TIME;
	}

	// 残り時間を減らす
	m_spinTime -= PER_SEC(1.0f) * timeScale;
	m_dashTime -= PER_SEC(1.0f) * timeScale;

	// 頭の向きへ進む
	float speed = GetSpeed();
	if (pAction->IsDriftHold()) {
		speed *= DRIFT_SLOW;
	}
	float rad = m_angle * DEG_TO_RAD;
	DirectX::XMFLOAT2 move;
	move.x = cosf(rad) * PER_SEC(speed) * timeScale;
	move.y = sinf(rad) * PER_SEC(speed) * timeScale;
	m_body.Move(move, pWalls, pRocks);	// 壁・岩に沿って滑る

	// アニメーション(10フレームごとに次のコマ)
	m_animFrame++;
	m_image.numU = (float)((m_animFrame / 10) % 3);
}

void SampleFish::UpdateAngle(Action* pAction, Camera* pCamera, float timeScale)
{
	// 向かいたい向き(優先順：方向キー、スティック、マウス)
	DirectX::XMFLOAT2 dir = pAction->GetMoveKeyDir();
	if (dir.x == 0.0f && dir.y == 0.0f) {
		dir = pAction->GetStickDir();
	}
	if (dir.x == 0.0f && dir.y == 0.0f && pAction->IsMouseMoved()) {
		// 魚からカーソル(ワールド座標)への向き
		DirectX::XMFLOAT2 cursor = pCamera->ScreenToWorld(pAction->GetMousePos());
		DirectX::XMFLOAT2 pos = m_body.GetPos();
		float dx = cursor.x - pos.x;
		float dy = cursor.y - pos.y;
		// 近すぎると向きがぶれるので変えない
		if (dx * dx + dy * dy > MOUSE_DEADZONE * MOUSE_DEADZONE) {
			dir = { dx, dy };
		}
	}
	if (dir.x == 0.0f && dir.y == 0.0f) {
		return; // どれも入力がない
	}

	// 今の向きと向かいたい向きの差(-180〜180度)
	float targetAngle = atan2f(dir.y, dir.x) * RAD_TO_DEG;
	float diff = targetAngle - m_angle;
	while (diff > 180.0f) diff -= 360.0f;
	while (diff < -180.0f) diff += 360.0f;

	// 1フレームに回れる角度まで(FISH_TURN_SPEED)
	float turn = PER_SEC(FISH_TURN_SPEED) * timeScale;
	if (diff > turn) diff = turn;
	if (diff < -turn) diff = -turn;
	m_angle += diff;

	// 角度を -180〜180 に収める
	if (m_angle > 180.0f) m_angle -= 360.0f;
	if (m_angle < -180.0f) m_angle += 360.0f;
}

void SampleFish::Draw(Camera* pCamera)
{
	// 回転アタック中は赤くする
	if (IsSpinning()) {
		m_image.SetColor(1.0f, 0.4f, 0.4f, 1.0f);
	}
	else {
		m_image.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	}
	m_image.SetAngle(m_angle);
	DirectX::XMFLOAT2 pos = m_body.GetPos();
	pCamera->Draw(&m_image, pos.x, pos.y, FISH_SIZE, FISH_SIZE);
}

void SampleFish::DrawDebug(DrawUtil* pDraw, DirectX::XMFLOAT4 color)
{
	pDraw->DrawCollider(GetCollider(), color);

	// 頭の向きを線で表示する
	DirectX::XMFLOAT2 pos = m_body.GetPos();
	float rad = m_angle * DEG_TO_RAD;
	float lineLength = FISH_RADIUS * 2.0f;
	pDraw->DrawLine(pos.x, pos.y, pos.x + cosf(rad) * lineLength, pos.y + sinf(rad) * lineLength, 2.0f, { 1.0f, 1.0f, 1.0f, 0.9f });
}

void SampleFish::Uninit()
{
	m_image.Uninit();
}

Collider SampleFish::GetCollider()
{
	return m_body.GetCollider();
}

DirectX::XMFLOAT2 SampleFish::GetPos()
{
	return m_body.GetPos();
}

float SampleFish::GetAngle()
{
	return m_angle;
}

float SampleFish::GetSpeed()
{
	float speed = FISH_SPEED;
	if (m_dashTime > 0.0f) {
		speed *= DASH_SPEED;
	}
	return speed;
}

bool SampleFish::IsSpinning()
{
	return m_spinTime > 0.0f;
}

void SampleFish::StopSpin()
{
	m_spinTime = 0.0f;
}
