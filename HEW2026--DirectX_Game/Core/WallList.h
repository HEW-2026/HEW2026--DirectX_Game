#pragma once
//==========================================================//
// 【WallList.h】壁(線と円)をまとめて持つ
//==========================================================//
//
// 壁は「線」と「円」で表す。
//   線：タイルの壁。Stage がタイルから自動で作るので、普段は自分で AddLine しなくてよい
//   円：置き岩やボス部屋の柱など、動かない丸い物。壁と同じで、通り抜けられない
// Body::Move に渡すと、Body はこれにぶつかって壁に沿って滑る。
//
// 使い方
//   m_rocks.Clear();                                // 毎フレーム作り直してよい(壊れた岩は足さない)
//   m_rocks.AddCircle({ 1250.0f, 420.0f }, 60.0f);  // 中心と半径(ドット)
//   m_body.Move(move, m_stage.GetWalls(), &m_rocks); // タイルの壁と岩の両方にぶつかる
//
#include <DirectXMath.h>

#define WALL_LINE_MAX	(4096)	// 壁の線の最大数
#define WALL_CIRCLE_MAX	(64)	// 壁の円の最大数

// 壁の線1本
struct WallLine
{
	DirectX::XMFLOAT2 start; // 始点(ドット)
	DirectX::XMFLOAT2 end;   // 終点(ドット)
};

// 壁の円1つ
struct WallCircle
{
	DirectX::XMFLOAT2 center;	// 中心(ドット)
	float radius;				// 半径(ドット)
};

// DrawUtil の前方宣言(ポインタだけ使うので、中身は .cpp で include する)
class DrawUtil;

class WallList
{
public:
	void Clear();	// 全部消す
	void AddLine(DirectX::XMFLOAT2 start, DirectX::XMFLOAT2 end);	// 線を足す(同じ向きでつながる線とは1本にまとめる)
	void AddCircle(DirectX::XMFLOAT2 center, float radius);		// 円を足す

	int GetLineCount();				// 線の数
	WallLine GetLine(int index);	// index番目の線
	int GetCircleCount();			// 円の数
	WallCircle GetCircle(int index);	// index番目の円

	void DrawDebug(DrawUtil* pDraw); // 全部の線と円をデバッグ表示する

private:
	WallLine m_line[WALL_LINE_MAX];			// 線
	int m_lineCount = 0;					// 線の数
	WallCircle m_circle[WALL_CIRCLE_MAX];	// 円
	int m_circleCount = 0;					// 円の数
	bool m_isFullMessage = false;			// 「多すぎる」のエラーを表示したか(何回も出さないため)
};
