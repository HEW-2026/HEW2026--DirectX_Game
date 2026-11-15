#include "Body.h"
#include <math.h>

#define BODY_ALLOW_INSIDE (0.5f) // めり込みとみなさない誤差(ドット)

// 線の上で、pointに一番近い点
static DirectX::XMFLOAT2 GetNearPointOnLine(DirectX::XMFLOAT2 point, WallLine line)
{
	float lineX = line.end.x - line.start.x;
	float lineY = line.end.y - line.start.y;
	float length2 = lineX * lineX + lineY * lineY; // 線の長さの2乗

	// 一番近い点の位置(0:始点、1:終点。0〜1に収める)
	float t = 0.0f;
	if (length2 > 0.0f) {
		t = ((point.x - line.start.x) * lineX + (point.y - line.start.y) * lineY) / length2;
		if (t < 0.0f) t = 0.0f;
		if (t > 1.0f) t = 1.0f;
	}
	return { line.start.x + lineX * t, line.start.y + lineY * t };
}

void Body::Init(float x, float y, float radius)
{
	m_pos = { x, y };
	m_radius = radius;
}

bool Body::Move(DirectX::XMFLOAT2 move, WallList* pWalls, WallList* pWalls2)
{
	// 1歩の長さ(BODY_STEP_MAX と半径の半分の小さい方。小さい体のすり抜けを防ぐ)
	float stepLength = BODY_STEP_MAX;
	if (stepLength > m_radius * 0.5f) {
		stepLength = m_radius * 0.5f;
	}
	if (stepLength <= 0.0f) {
		stepLength = BODY_STEP_MAX;
	}

	// 動く距離を1歩の長さで区切る
	float length = sqrtf(move.x * move.x + move.y * move.y);
	// 1フレームの上限(1歩の長さ × BODY_STEP_LIMIT)を超える移動はしない
	if (!(length <= stepLength * BODY_STEP_LIMIT)) {
		return false;
	}
	int stepCount = (int)ceilf(length / stepLength);
	if (stepCount < 1) {
		stepCount = 1;
	}
	if (stepCount > BODY_STEP_LIMIT) {
		stepCount = BODY_STEP_LIMIT;
	}
	float stepX = move.x / stepCount;
	float stepY = move.y / stepCount;

	bool isHitWall = false;
	for (int i = 0; i < stepCount; i++) {
		// 1歩進む
		DirectX::XMFLOAT2 oldPos = m_pos;
		m_pos.x += stepX;
		m_pos.y += stepY;

		// 壁から押し出す(角で別の壁に押し戻されることがあるので2回)
		for (int k = 0; k < 2; k++) {
			if (PushOutOfWalls(pWalls)) isHitWall = true;
			if (PushOutOfWalls(pWalls2)) isHitWall = true;
		}

		// それでも壁にめり込んでいたら、1歩前に戻して止まる
		if (IsInWall(pWalls) || IsInWall(pWalls2)) {
			m_pos = oldPos;
			break;
		}
	}
	return isHitWall;
}

bool Body::PushOutOfWalls(WallList* pWalls)
{
	if (pWalls == nullptr) {
		return false;
	}

	bool isPushed = false;

	// 線の壁
	for (int i = 0; i < pWalls->GetLineCount(); i++) {
		// 壁の上で一番近い点
		DirectX::XMFLOAT2 nearPoint = GetNearPointOnLine(m_pos, pWalls->GetLine(i));
		float dx = m_pos.x - nearPoint.x;
		float dy = m_pos.y - nearPoint.y;
		float distance = sqrtf(dx * dx + dy * dy);

		// 半径より近ければ、半径の距離まで壁と直角に離す
		if (distance < m_radius && distance > 0.0f) {
			m_pos.x = nearPoint.x + dx / distance * m_radius;
			m_pos.y = nearPoint.y + dy / distance * m_radius;
			isPushed = true;
		}
	}

	// 円の壁(中心間の距離が半径の合計になるまで離す)
	for (int i = 0; i < pWalls->GetCircleCount(); i++) {
		WallCircle circle = pWalls->GetCircle(i);
		float dx = m_pos.x - circle.center.x;
		float dy = m_pos.y - circle.center.y;
		float distance = sqrtf(dx * dx + dy * dy);
		float minDistance = m_radius + circle.radius;
		if (distance < minDistance && distance > 0.0f) {
			m_pos.x = circle.center.x + dx / distance * minDistance;
			m_pos.y = circle.center.y + dy / distance * minDistance;
			isPushed = true;
		}
	}
	return isPushed;
}

bool Body::IsInWall(WallList* pWalls)
{
	if (pWalls == nullptr) {
		return false;
	}

	// 線の壁
	for (int i = 0; i < pWalls->GetLineCount(); i++) {
		DirectX::XMFLOAT2 nearPoint = GetNearPointOnLine(m_pos, pWalls->GetLine(i));
		float dx = m_pos.x - nearPoint.x;
		float dy = m_pos.y - nearPoint.y;
		float limit = m_radius - BODY_ALLOW_INSIDE;
		if (dx * dx + dy * dy < limit * limit) {
			return true;
		}
	}

	// 円の壁
	for (int i = 0; i < pWalls->GetCircleCount(); i++) {
		WallCircle circle = pWalls->GetCircle(i);
		float dx = m_pos.x - circle.center.x;
		float dy = m_pos.y - circle.center.y;
		float limit = m_radius + circle.radius - BODY_ALLOW_INSIDE;
		if (dx * dx + dy * dy < limit * limit) {
			return true;
		}
	}
	return false;
}

void Body::SetPos(float x, float y)
{
	m_pos = { x, y };
}

DirectX::XMFLOAT2 Body::GetPos()
{
	return m_pos;
}

float Body::GetRadius()
{
	return m_radius;
}

Collider Body::GetCollider()
{
	Collider collider;
	collider.SetCircle(m_radius);
	collider.SetPos(m_pos.x, m_pos.y);
	return collider;
}
