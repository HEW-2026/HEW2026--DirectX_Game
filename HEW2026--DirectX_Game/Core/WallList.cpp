#include "WallList.h"
#include "DrawUtil.h"
#include <Windows.h> // MessageBoxA

void WallList::Clear()
{
	m_lineCount = 0;
	m_circleCount = 0;
}

void WallList::AddLine(DirectX::XMFLOAT2 start, DirectX::XMFLOAT2 end)
{
	// 始点をそろえる(xが小さい方。xが同じならyが小さい方)
	if (start.x > end.x || (start.x == end.x && start.y > end.y)) {
		DirectX::XMFLOAT2 work = start;
		start = end;
		end = work;
	}

	// 同じ向きでつながる線は1本にまとめる(タイルの継ぎ目で引っかからないように)
	float dirX = end.x - start.x;
	float dirY = end.y - start.y;
	for (int i = 0; i < m_lineCount; i++) {
		// 平行か(外積が0)。タイルの角は整数なので == で比べる
		float lineDirX = m_line[i].end.x - m_line[i].start.x;
		float lineDirY = m_line[i].end.y - m_line[i].start.y;
		if (dirX * lineDirY - dirY * lineDirX != 0.0f) {
			continue;
		}

		// 前の線の終点 = 新しい線の始点 → 前の線を伸ばす
		if (m_line[i].end.x == start.x && m_line[i].end.y == start.y) {
			m_line[i].end = end;
			return;
		}
		// 新しい線の終点 = 前の線の始点 → 前の線を伸ばす
		if (end.x == m_line[i].start.x && end.y == m_line[i].start.y) {
			m_line[i].start = start;
			return;
		}
	}

	// まとめられなければ新しく足す
	if (m_lineCount >= WALL_LINE_MAX) {
		if (!m_isFullMessage) {
			MessageBoxA(NULL, "壁の線が多すぎます。WALL_LINE_MAXを増やしてください。", "エラー", MB_ICONERROR | MB_OK);
			m_isFullMessage = true;
		}
		return;
	}
	m_line[m_lineCount].start = start;
	m_line[m_lineCount].end = end;
	m_lineCount++;
}

void WallList::AddCircle(DirectX::XMFLOAT2 center, float radius)
{
	if (m_circleCount >= WALL_CIRCLE_MAX) {
		if (!m_isFullMessage) {
			MessageBoxA(NULL, "壁の円が多すぎます。WALL_CIRCLE_MAXを増やしてください。", "エラー", MB_ICONERROR | MB_OK);
			m_isFullMessage = true;
		}
		return;
	}
	m_circle[m_circleCount].center = center;
	m_circle[m_circleCount].radius = radius;
	m_circleCount++;
}

int WallList::GetLineCount()
{
	return m_lineCount;
}

WallLine WallList::GetLine(int index)
{
	return m_line[index];
}

int WallList::GetCircleCount()
{
	return m_circleCount;
}

WallCircle WallList::GetCircle(int index)
{
	return m_circle[index];
}

void WallList::DrawDebug(DrawUtil* pDraw)
{
	DirectX::XMFLOAT4 color = { 1.0f, 0.5f, 0.0f, 0.9f }; // オレンジ
	for (int i = 0; i < m_lineCount; i++) {
		WallLine line = m_line[i];
		pDraw->DrawLine(line.start.x, line.start.y, line.end.x, line.end.y, 2.0f, color);
	}
	for (int i = 0; i < m_circleCount; i++) {
		WallCircle circle = m_circle[i];
		pDraw->DrawCircle(circle.center.x, circle.center.y, circle.radius, { 1.0f, 0.5f, 0.0f, 0.3f });
	}
}
