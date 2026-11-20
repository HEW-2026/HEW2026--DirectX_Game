#include "DrawList.h"
#include <Windows.h> // MessageBoxA

DrawList g_drawList;

void DrawList::Add(Object* pObject, DRAW_LAYER layer)
{
	// いっぱいの時は予約せずにすぐ描く(順番は崩れるが、消えるよりはよい)
	if (m_count >= DRAW_LIST_MAX) {
		if (!m_isFullMessage) {
			MessageBoxA(NULL, "描く物が多すぎます。DRAW_LIST_MAXを増やしてください。", "エラー", MB_ICONERROR | MB_OK);
			m_isFullMessage = true;
		}
		pObject->Draw();
		return;
	}

	// 今の状態を写しておく
	DrawItem item;
	item.pObject = pObject;
	item.layer = layer;
	item.pos = pObject->GetPos();
	item.size = pObject->GetSize();
	item.angle = pObject->GetAngle();
	item.color = pObject->GetColor();
	item.numU = pObject->numU;
	item.numV = pObject->numV;
	m_item[m_count] = item;
	m_count++;
}

void DrawList::DrawAll()
{
	// 奥のレイヤーから順に描く(同じレイヤーの中は予約した順)
	for (int layer = 0; layer < DRAW_LAYER_MAX; layer++) {
		for (int i = 0; i < m_count; i++) {
			if (m_item[i].layer != layer) continue;
			DrawItem item = m_item[i];

			// 予約した時の状態に戻してから描く
			Object* pObject = item.pObject;
			pObject->SetPos(item.pos.x, item.pos.y, item.pos.z);
			pObject->SetSize(item.size.x, item.size.y, item.size.z);
			pObject->SetAngle(item.angle);
			pObject->SetColor(item.color.x, item.color.y, item.color.z, item.color.w);
			pObject->numU = item.numU;
			pObject->numV = item.numV;
			pObject->Draw();
		}
	}
	m_count = 0;
}
