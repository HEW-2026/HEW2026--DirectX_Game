#include "Collider.h"
#include "Config.h"
#include <math.h>

void Collider::SetCircle(float radius)
{
	m_type = COLLIDER_CIRCLE;
	m_radiusX = radius;
	m_radiusY = radius;
}

void Collider::SetEllipse(float radiusX, float radiusY)
{
	m_type = COLLIDER_ELLIPSE;
	m_radiusX = radiusX;
	m_radiusY = radiusY;
}

void Collider::SetPos(float x, float y) { m_pos = { x,y }; }
void Collider::SetAngle(float angle) { m_angle = angle; }
void Collider::SetTag(TAG tag) { m_tag = tag; }
void Collider::SetHitLayer(HIT_LAYER layer) { m_hitLayer = layer; }

COLLIDER_TYPE Collider::GetType() { return m_type; }
DirectX::XMFLOAT2 Collider::GetPos() { return m_pos; }
float Collider::GetRadiusX() { return m_radiusX; }
float Collider::GetRadiusY() { return m_radiusY; }
float Collider::GetAngle() { return m_angle; }
TAG Collider::GetTag() { return m_tag; }
bool Collider::CompareTag(TAG tag) { return m_tag == tag; }
HIT_LAYER Collider::GetHitLayer() { return m_hitLayer; }

DirectX::XMFLOAT2 Collider::GetEdgePoint(DirectX::XMFLOAT2 point)
{
	//	中心からpointへの角度(ラジアン)
	float worldDir = atan2f(point.y - m_pos.y, point.x - m_pos.x);

	//	楕円の回転を引いた角度
	float localDir = worldDir - m_angle * DEG_TO_RAD;

	//	中心から縁までの距離(楕円の式から求める)
	float cx = cosf(localDir) / m_radiusX;
	float sy = sinf(localDir) / m_radiusY;
	float length = 1.0f / sqrtf(cx * cx + sy * sy);

	return { m_pos.x + cosf(worldDir) * length, m_pos.y + sinf(worldDir) * length };
}

//	円と円
static bool HitCircleCircle(Collider a, Collider b)
{
	//	中心間の距離 < 半径の合計 なら当たり(2乗で比べる)
	float dx = b.GetPos().x - a.GetPos().x;
	float dy = b.GetPos().y - a.GetPos().y;
	float r = a.GetRadiusX() + b.GetRadiusX();
	return dx * dx + dy * dy < r * r;
}

//	円と楕円
static bool HitCircleEllipse(Collider circle, Collider ellipse)
{
	//	円の中心を、楕円の回転を戻した座標に直す
	float rad = ellipse.GetAngle() * DEG_TO_RAD;
	float dx = circle.GetPos().x - ellipse.GetPos().x;
	float dy = circle.GetPos().y - ellipse.GetPos().y;
	float localX = dx * cosf(rad) + dy * sinf(rad);
	float localY = -dx * sinf(rad) + dy * cosf(rad);

	//	楕円の半径に円の半径を足した楕円の中に、円の中心があれば当たり(プロトタイプと同じ計算)
	float sizeX = ellipse.GetRadiusX() + circle.GetRadiusX();
	float sizeY = ellipse.GetRadiusY() + circle.GetRadiusX();
	return (localX / sizeX) * (localX / sizeX) + (localY / sizeY) * (localY / sizeY) <= 1.0f;
}

bool IsHit(Collider a, Collider b)
{
	//	当たらないレイヤーの組み合わせなら調べない
	if (!IsLayerHit(a.GetHitLayer(), b.GetHitLayer())) {
		return false;
	}

	if (a.GetType() == COLLIDER_CIRCLE && b.GetType() == COLLIDER_CIRCLE) {
		return HitCircleCircle(a, b);
	}
	if (a.GetType() == COLLIDER_CIRCLE && b.GetType() == COLLIDER_ELLIPSE) {
		return HitCircleEllipse(a, b);
	}
	if (a.GetType() == COLLIDER_ELLIPSE && b.GetType() == COLLIDER_CIRCLE) {
		return HitCircleEllipse(b, a);
	}
	//	楕円同士は未対応
	return false;
}

bool IsFront(DirectX::XMFLOAT2 myPos, float angle, Collider target)
{
	//	相手の縁のうち、自分に一番近い向きの点(大きい相手でも当たった場所で判定する)
	DirectX::XMFLOAT2 point = target.GetEdgePoint(myPos);

	//	自分の向き(長さ1)
	float rad = angle * DEG_TO_RAD;
	float dirX = cosf(rad);
	float dirY = sinf(rad);

	//	自分から縁の点への向き
	float toX = point.x - myPos.x;
	float toY = point.y - myPos.y;

	//	内積が正なら正面(向きの差が90度未満)
	return dirX * toX + dirY * toY > 0.0f;
}
