#include "Action.h"
#include "Renderer.h"
#include <math.h>

// キーの割り当て(キーボード・マウス)
#define KEY_DRIFT_MOUSE (VK_LBUTTON) // ドリフト(マウス)
#define KEY_DRIFT       (VK_SPACE)   // ドリフト(キーボード)
#define KEY_SPIN        (VK_RBUTTON) // 回転アタック
#define KEY_RETRY       ('R')        // やり直し
#define KEY_DECIDE      (VK_RETURN)  // 決定(左クリックとスペースも決定になる)
#define KEY_DEBUG       (VK_F1)      // デバッグ表示の切り替え

// ボタンの割り当て(Xboxコントローラー)
#define BUTTON_SPIN     (XINPUT_A)     // 回転アタック
#define BUTTON_RETRY    (XINPUT_BACK)  // やり直し(View)
#define BUTTON_DECIDE   (XINPUT_A)     // 決定
#define BUTTON_START    (XINPUT_START) // 決定(Menu)
#define TRIGGER_DRIFT_ON (0.5f)        // RTをこれ以上押し込んだらドリフト(0〜1)

void Action::Init(HWND hWnd, Input* pInput)
{
	m_hWnd = hWnd;
	m_pInput = pInput;

	// 起動時のカーソルの位置(マウスを動かしたかの判定用)
	GetCursorPos(&m_firstCursor);
	m_isMouseMoved = false;
}

void Action::Update()
{
	// ウィンドウが非アクティブの間は、入力をすべて無視する
	m_isActive = (GetForegroundWindow() == m_hWnd);
	if (!m_isActive) {
		// 「離した瞬間」も出さない(切り替えた時のダッシュ防止)
		m_drift = false;
		m_driftOld = false;
		// ウィンドウに戻る時のクリックを、ドリフトや決定にしない
		m_isDriftLock = true;
		m_ignoreFrame = ACTION_IGNORE_FRAME;
		return;
	}

	// ボタンを受け付けない残りフレームを減らす
	if (m_ignoreFrame > 0) {
		m_ignoreFrame--;
	}

	// ドリフト：マウス・キーボード・RTのどれか。全部離した時に「離した」とする
	m_driftOld = m_drift;
	m_drift = m_pInput->GetKeyPress(KEY_DRIFT_MOUSE)
		|| m_pInput->GetKeyPress(KEY_DRIFT)
		|| m_pInput->GetRightTrigger() >= TRIGGER_DRIFT_ON;

	// シーン切り替え前から押しているドリフトは、一度離すまで無視する(開始直後のダッシュ防止)
	if (m_isDriftLock) {
		if (!m_drift) {
			m_isDriftLock = false;
		}
		m_drift = false;
		m_driftOld = false;
	}

	UpdateMousePos();
}

void Action::ResetInput()
{
	m_isDriftLock = true;
	m_drift = false;
	m_driftOld = false;
	m_ignoreFrame = ACTION_IGNORE_FRAME;
}

bool Action::IsDriftHold()
{
	return m_drift;
}

bool Action::IsDriftRelease()
{
	// 1フレーム前は押していて、今は離している
	return m_driftOld && !m_drift;
}

bool Action::IsSpinTrigger()
{
	if (!m_isActive || m_ignoreFrame > 0) return false;
	return m_pInput->GetKeyTrigger(KEY_SPIN) || m_pInput->GetButtonTrigger(BUTTON_SPIN);
}

bool Action::IsRetryTrigger()
{
	if (!m_isActive || m_ignoreFrame > 0) return false;
	return m_pInput->GetKeyTrigger(KEY_RETRY) || m_pInput->GetButtonTrigger(BUTTON_RETRY);
}

bool Action::IsDecideTrigger()
{
	if (!m_isActive || m_ignoreFrame > 0) return false;
	return m_pInput->GetKeyTrigger(KEY_DECIDE)
		|| m_pInput->GetKeyTrigger(KEY_DRIFT_MOUSE)
		|| m_pInput->GetKeyTrigger(KEY_DRIFT)
		|| m_pInput->GetButtonTrigger(BUTTON_DECIDE)
		|| m_pInput->GetButtonTrigger(BUTTON_START);
}

bool Action::IsDebugTrigger()
{
	if (!m_isActive) return false;
	return m_pInput->GetKeyTrigger(KEY_DEBUG);
}

DirectX::XMFLOAT2 Action::GetMoveKeyDir()
{
	DirectX::XMFLOAT2 dir = { 0.0f, 0.0f };
	if (!m_isActive) return dir;

	// 押している方向を足す(上が+y)
	if (m_pInput->GetKeyPress(VK_UP) || m_pInput->GetKeyPress(VK_W)) dir.y += 1.0f;
	if (m_pInput->GetKeyPress(VK_DOWN) || m_pInput->GetKeyPress(VK_S)) dir.y -= 1.0f;
	if (m_pInput->GetKeyPress(VK_LEFT) || m_pInput->GetKeyPress(VK_A)) dir.x -= 1.0f;
	if (m_pInput->GetKeyPress(VK_RIGHT) || m_pInput->GetKeyPress(VK_D)) dir.x += 1.0f;

	// 斜めの時も長さを1にする
	float length = sqrtf(dir.x * dir.x + dir.y * dir.y);
	if (length > 0.0f) {
		dir.x /= length;
		dir.y /= length;
	}
	return dir;
}

DirectX::XMFLOAT2 Action::GetStickDir()
{
	DirectX::XMFLOAT2 dir = { 0.0f, 0.0f };
	if (!m_isActive) return dir;

	DirectX::XMFLOAT2 stick = m_pInput->GetLeftAnalogStick();
	float length = sqrtf(stick.x * stick.x + stick.y * stick.y);

	// デッドゾーン内は0(触っていなくても少し傾いているため)
	if (length < STICK_DEADZONE) {
		return dir;
	}
	// 長さが1を超えないようにする
	if (length > 1.0f) {
		stick.x /= length;
		stick.y /= length;
	}
	return stick;
}

DirectX::XMFLOAT2 Action::GetMousePos()
{
	return m_mousePos;
}

bool Action::IsMouseMoved()
{
	return m_isMouseMoved;
}

void Action::UpdateMousePos()
{
	// カーソルの位置(モニター座標)。取得できない時は前の位置のまま
	POINT pt;
	if (!GetCursorPos(&pt)) {
		return;
	}

	// 起動時の位置から動いたか
	if (pt.x != m_firstCursor.x || pt.y != m_firstCursor.y) {
		m_isMouseMoved = true;
	}

	// ウィンドウ内の座標に変換(左上が原点、下が+)
	ScreenToClient(m_hWnd, &pt);

	// ウィンドウの大きさ(最小化中は計算しない)
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	float width = (float)rc.right;
	float height = (float)rc.bottom;
	if (width <= 0.0f || height <= 0.0f) {
		return;
	}

	// 黒帯を除いた描画範囲(Renderer.cpp の ResizeWindow と同じ計算)
	float targetAspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
	float viewX = 0.0f;
	float viewY = 0.0f;
	float viewW = width;
	float viewH = height;
	if (width / height > targetAspect) {
		// 横長：左右に帯
		viewW = height * targetAspect;
		viewX = (width - viewW) / 2.0f;
	}
	else {
		// 縦長：上下に帯
		viewH = width / targetAspect;
		viewY = (height - viewH) / 2.0f;
	}

	// 画面座標に変換(中央が原点、上が+)
	m_mousePos.x = ((pt.x - viewX) / viewW - 0.5f) * SCREEN_WIDTH;
	m_mousePos.y = (0.5f - (pt.y - viewY) / viewH) * SCREEN_HEIGHT;
}
