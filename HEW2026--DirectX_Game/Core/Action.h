#pragma once
//==========================================================//
// 【Action.h】操作(キーボード・マウス・Xboxコントローラー)をまとめて管理する
//==========================================================//
//
// ゲームの処理では、キーやボタンを直接読まずに Action の関数を使う。
// 割り当てを変える時は Action.cpp の上にある #define だけを変えればよい。
//
// 今の割り当て
//   向き        ：マウスカーソル／方向キー・WASD／左スティック
//   ドリフト    ：左クリック／スペース／RT(R2)        (押している間)
//   回転アタック：右クリック／A(×)
//   やり直し    ：R／View(Create)
//   決定        ：左クリック／スペース／Enter／A(×)／Menu(Options)
//   デバッグ表示：F1
//
// Update は Game が毎フレーム呼ぶ。シーンやキャラからは呼ばない(2回呼ぶと「離した瞬間」が取れなくなる)。
//
// 使い方
//   m_pAction->ResetInput();                      // シーンの Init で呼ぶ(前のシーンで押したボタンを持ち越さない)
//   if (m_pAction->IsSpinTrigger()) { ... }       // 回転アタックを押した瞬間
//   DirectX::XMFLOAT2 dir = m_pAction->GetMoveKeyDir(); // 方向キーの向き(押していなければ0,0)
//
// ウィンドウが非アクティブの間は、入力なしとして扱う。
//
#include "input.h"

#define STICK_DEADZONE		(0.25f)	// スティックの無効範囲(0〜1)
#define ACTION_IGNORE_FRAME	(20)	// ResetInput 後にボタンを無視するフレーム数(連打で画面を飛ばさない)

class Action
{
public:
	void Init(HWND hWnd, Input* pInput); // 初期化
	void Update();                       // 更新(Game が input.Update() の後に呼ぶ)
	void ResetInput();                   // 押しているボタンを無効にする(シーンの Init で呼ぶ)

	bool IsDriftHold();    // ドリフト(押している間)
	bool IsDriftRelease(); // ドリフトを離した瞬間
	bool IsSpinTrigger();  // 回転アタック(押した瞬間)
	bool IsRetryTrigger(); // やり直し(押した瞬間)
	bool IsDecideTrigger(); // 決定(押した瞬間。タイトルやリザルトで使う)
	bool IsDebugTrigger();  // デバッグ表示の切り替え(押した瞬間)

	DirectX::XMFLOAT2 GetMoveKeyDir(); // 方向キー・WASDの向き(長さ1。押していなければ0,0)
	DirectX::XMFLOAT2 GetStickDir();   // 左スティックの向きと倒し具合(長さ0〜1。デッドゾーン内は0,0)
	DirectX::XMFLOAT2 GetMousePos();   // マウスカーソルの位置(画面座標：画面中央が原点、上が+)
	bool IsMouseMoved();               // 起動してから一度でもマウスを動かしたか

private:
	HWND m_hWnd = NULL;         // ウィンドウ
	Input* m_pInput = nullptr;  // キー入力
	bool m_isActive = false;    // ウィンドウが選ばれているか
	bool m_drift = false;       // 今のフレームでドリフトを押しているか
	bool m_driftOld = false;    // 1フレーム前にドリフトを押していたか
	bool m_isDriftLock = false; // ドリフトを一度離すまで受け付けないか(シーンが変わった直後)
	int m_ignoreFrame = 0;      // 決定などのボタンを受け付けない残りフレーム数
	DirectX::XMFLOAT2 m_mousePos = { 0.0f, 0.0f }; // マウスカーソルの位置
	POINT m_firstCursor = { 0, 0 };	// 起動した時のカーソルの位置(動かしたかを調べるため)
	bool m_isMouseMoved = false;	// 一度でもマウスを動かしたか

	void UpdateMousePos(); // マウスカーソルの位置を計算する
};
