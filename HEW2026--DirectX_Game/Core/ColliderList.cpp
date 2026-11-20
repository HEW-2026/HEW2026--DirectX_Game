#include "ColliderList.h"
#include <Windows.h> // MessageBoxA

void ColliderList::Clear()
{
	m_count = 0;
}

void ColliderList::Add(Collider collider, int ownerIndex)
{
	if (m_count >= COLLIDER_LIST_MAX) {
		// メッセージは1回だけ出す
		if (!m_isFullMessage) {
			MessageBoxA(NULL, "当たり判定が多すぎます。COLLIDER_LIST_MAXを増やしてください。", "エラー", MB_ICONERROR | MB_OK);
			m_isFullMessage = true;
		}
		return;
	}
	m_item[m_count].collider = collider;
	m_item[m_count].ownerIndex = ownerIndex;
	m_count++;
}

int ColliderList::GetCount()
{
	return m_count;
}

ColliderItem ColliderList::GetItem(int index)
{
	// 範囲外は空の物を返す(FindWithTag の -1 をそのまま渡しても壊れない)
	if (index < 0 || index >= m_count) {
		ColliderItem empty;
		empty.ownerIndex = -1;
		return empty;
	}
	return m_item[index];
}

int ColliderList::FindWithTag(TAG tag)
{
	for (int i = 0; i < m_count; i++) {
		if (m_item[i].collider.CompareTag(tag)) {
			return i;
		}
	}
	return -1;
}

int ColliderList::FindAllWithTag(TAG tag, int result[COLLIDER_LIST_MAX])
{
	int count = 0;
	for (int i = 0; i < m_count; i++) {
		if (m_item[i].collider.CompareTag(tag)) {
			result[count] = i;
			count++;
		}
	}
	return count;
}
