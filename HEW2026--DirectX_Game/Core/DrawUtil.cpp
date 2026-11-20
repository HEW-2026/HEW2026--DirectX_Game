#include "DrawUtil.h"
#include "Camera.h"
#include "Config.h"
#include <math.h>

void DrawUtil::Init(Camera* pCamera, bool isDebug)
{
	m_pCamera = pCamera;
	m_isDebug = isDebug;
	m_layer = DRAW_LAYER_DEBUG;
	if (!isDebug) {
		m_layer = DRAW_LAYER_EFFECT_FRONT;
	}
	m_circle.Init("asset/debug_circle.png");
	m_box.Init("asset/debug_box.png");
}

void DrawUtil::Uninit()
{
	m_circle.Uninit();
	m_box.Uninit();
}

void DrawUtil::SwitchShow()
{
	m_isShow = !m_isShow;
}

void DrawUtil::SetLayer(DRAW_LAYER layer)
{
	m_layer = layer;
}

bool DrawUtil::IsShow()
{
	// 演出用はいつも描く
	if (!m_isDebug) {
		return true;
	}
	// デバッグ用は、DEBUG_DRAW が1で、F1で表示にしている時だけ描く
	return DEBUG_DRAW && m_isShow;
}

void DrawUtil::DrawCircle(float x, float y, float radius, DirectX::XMFLOAT4 color)
{
	if (!IsShow()) return;

	m_circle.SetAngle(0.0f);
	m_circle.SetColor(color.x, color.y, color.z, color.w);
	m_pCamera->Draw(&m_circle, x, y, radius * 2.0f, radius * 2.0f, m_layer);	//	画像の大きさは直径
}

void DrawUtil::DrawEllipse(float x, float y, float radiusX, float radiusY, float angle, DirectX::XMFLOAT4 color)
{
	if (!IsShow()) return;

	m_circle.SetAngle(angle);
	m_circle.SetColor(color.x, color.y, color.z, color.w);
	m_pCamera->Draw(&m_circle, x, y, radiusX * 2.0f, radiusY * 2.0f, m_layer);	//	円の画像を引き伸ばして楕円にする
}

void DrawUtil::DrawBox(float x, float y, float width, float height, DirectX::XMFLOAT4 color)
{
	if (!IsShow()) return;

	m_box.SetAngle(0.0f);
	m_box.SetColor(color.x, color.y, color.z, color.w);
	m_pCamera->Draw(&m_box, x, y, width, height, m_layer);
}

void DrawUtil::DrawLine(float x1, float y1, float x2, float y2, float thickness, DirectX::XMFLOAT4 color)
{
	if (!IsShow()) return;

	//	始点と終点の差から、長さと角度を求める
	float dx = x2 - x1;
	float dy = y2 - y1;
	float length = sqrtf(dx * dx + dy * dy);
	float angle = atan2f(dy, dx) * RAD_TO_DEG;	//	ラジアン → 度

	//	細長い四角を、2点の真ん中に置いて回転させる
	m_box.SetAngle(angle);
	m_box.SetColor(color.x, color.y, color.z, color.w);
	m_pCamera->Draw(&m_box, (x1 + x2) / 2.0f, (y1 + y2) / 2.0f, length, thickness, m_layer);
}

void DrawUtil::DrawCollider(Collider collider, DirectX::XMFLOAT4 color)
{
	DirectX::XMFLOAT2 pos = collider.GetPos();
	DrawEllipse(pos.x, pos.y, collider.GetRadiusX(), collider.GetRadiusY(), collider.GetAngle(), color);
}
