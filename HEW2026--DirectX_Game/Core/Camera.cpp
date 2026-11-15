#include "Camera.h"
#include "Config.h"
#include <math.h>
#include <stdlib.h> // rand

void Camera::Init()
{
	m_pos = { 0.0f, 0.0f };
	m_target = { 0.0f, 0.0f };
	m_shakeOffset = { 0.0f, 0.0f };
	m_viewScale = 1.0f;
	m_viewScaleTarget = 1.0f;
	m_followRate = CAMERA_FOLLOW_RATE;
	m_shakePower = 0.0f;
	m_shakeFrame = 0;
	m_shakeFrameMax = 0;
	m_useBounds = false;
}

void Camera::Update()
{
	// 追いかける位置へ近づく(1フレームで残りの距離の followRate/60)
	m_pos.x += (m_target.x - m_pos.x) * PER_SEC(m_followRate);
	m_pos.y += (m_target.y - m_pos.y) * PER_SEC(m_followRate);

	// 映す範囲の倍率を目標へ近づける(引く時は速く、戻る時は遅く)
	float rate = CAMERA_ZOOM_IN_RATE;
	if (m_viewScaleTarget > m_viewScale) {
		rate = CAMERA_ZOOM_OUT_RATE;
	}
	m_viewScale += (m_viewScaleTarget - m_viewScale) * PER_SEC(rate);

	// ステージの外を映さないようにする
	KeepInBounds();

	// 揺れ(残りフレームに比例して小さくする)
	m_shakeOffset = { 0.0f, 0.0f };
	if (m_shakeFrame > 0) {
		float power = m_shakePower * m_shakeFrame / m_shakeFrameMax;
		// 揺れ幅 × -1〜1 の乱数
		m_shakeOffset.x = power * (rand() / (float)RAND_MAX * 2.0f - 1.0f);
		m_shakeOffset.y = power * (rand() / (float)RAND_MAX * 2.0f - 1.0f);
		m_shakeFrame--;
	}
}

void Camera::SetTarget(float x, float y)
{
	m_target = { x, y };
}

void Camera::SetPos(float x, float y)
{
	m_pos = { x, y };
	m_target = { x, y };
	KeepInBounds();
}

void Camera::SetFollowRate(float rate)
{
	m_followRate = rate;
}

void Camera::SetViewScale(float scale)
{
	m_viewScaleTarget = scale;
}

void Camera::Shake(float power, float second)
{
	m_shakePower = power;
	m_shakeFrame = (int)(second * FPS + 0.5f); // 秒 → フレーム数(四捨五入)
	m_shakeFrameMax = m_shakeFrame;
}

void Camera::SetBounds(float left, float bottom, float right, float top)
{
	m_useBounds = true;
	m_boundLeft = left;
	m_boundBottom = bottom;
	m_boundRight = right;
	m_boundTop = top;
}

DirectX::XMFLOAT2 Camera::WorldToScreen(DirectX::XMFLOAT2 world)
{
	// カメラからの位置 × 拡大率
	float scale = GetScreenScale();
	float x = (world.x - m_pos.x - m_shakeOffset.x) * scale;
	float y = (world.y - m_pos.y - m_shakeOffset.y) * scale;

	// ピクセル単位に丸める(ドット絵のにじみ防止)
	return { floorf(x + 0.5f), floorf(y + 0.5f) };
}

DirectX::XMFLOAT2 Camera::ScreenToWorld(DirectX::XMFLOAT2 screen)
{
	// WorldToScreen の逆の計算
	float scale = GetScreenScale();
	return { screen.x / scale + m_pos.x + m_shakeOffset.x, screen.y / scale + m_pos.y + m_shakeOffset.y };
}

float Camera::WorldToScreenLength(float length)
{
	return length * GetScreenScale();
}

void Camera::Draw(Object* pObject, float x, float y, float width, float height)
{
	// 左下と右上の角をピクセルに丸めてから大きさを決める(引いた時のタイルのすき間防止)
	DirectX::XMFLOAT2 leftBottom = WorldToScreen({ x - width / 2.0f, y - height / 2.0f });
	DirectX::XMFLOAT2 rightTop = WorldToScreen({ x + width / 2.0f, y + height / 2.0f });
	pObject->SetPos((leftBottom.x + rightTop.x) / 2.0f, (leftBottom.y + rightTop.y) / 2.0f, 0.0f);
	pObject->SetSize(rightTop.x - leftBottom.x, rightTop.y - leftBottom.y, 0.0f);
	pObject->Draw();
}

float Camera::GetViewLeft()
{
	return m_pos.x - VIEW_WIDTH / 2.0f * m_viewScale;
}

float Camera::GetViewRight()
{
	return m_pos.x + VIEW_WIDTH / 2.0f * m_viewScale;
}

float Camera::GetViewBottom()
{
	return m_pos.y - VIEW_HEIGHT / 2.0f * m_viewScale;
}

float Camera::GetViewTop()
{
	return m_pos.y + VIEW_HEIGHT / 2.0f * m_viewScale;
}

float Camera::GetScreenScale()
{
	// 1ドットが画面で何ピクセルか(普段は PIXEL_SCALE。引くほど小さい)
	return PIXEL_SCALE / m_viewScale;
}

void Camera::KeepInBounds()
{
	if (!m_useBounds) {
		return;
	}

	// 画面の半分の大きさ(ワールド座標)
	float halfW = VIEW_WIDTH / 2.0f * m_viewScale;
	float halfH = VIEW_HEIGHT / 2.0f * m_viewScale;

	// 横：ステージが画面より狭ければ中央、広ければ端で止める
	if (m_boundRight - m_boundLeft <= halfW * 2.0f) {
		m_pos.x = (m_boundLeft + m_boundRight) / 2.0f;
	}
	else {
		if (m_pos.x < m_boundLeft + halfW) m_pos.x = m_boundLeft + halfW;
		if (m_pos.x > m_boundRight - halfW) m_pos.x = m_boundRight - halfW;
	}

	// 縦：横と同じ
	if (m_boundTop - m_boundBottom <= halfH * 2.0f) {
		m_pos.y = (m_boundBottom + m_boundTop) / 2.0f;
	}
	else {
		if (m_pos.y < m_boundBottom + halfH) m_pos.y = m_boundBottom + halfH;
		if (m_pos.y > m_boundTop - halfH) m_pos.y = m_boundTop - halfH;
	}
}
