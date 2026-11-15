#pragma once
//==========================================================//
// 【Stage.h】タイルのステージ(CSVの読み込み・描画・壁の線・物の配置)
//==========================================================//
//
// 座標はワールド座標(ドット。ステージの左下が原点、右が+x、上が+y)。
// CSVの1行目がステージの一番上になる。
//
// 使い方
//   m_stage.Init("asset/Stage/sample.csv");      // 画像を読み込み、ステージを読み込む
//   m_stage.Load("asset/Stage/stage02.csv");     // 別のステージに変える(画像はそのまま)
//   m_stage.Draw(&m_camera);                     // 画面に映っているタイルだけ描く
//   m_body.Move(move, m_stage.GetWalls());       // 壁にぶつかる
//   if (m_stage.IsSolid(x, y)) { ... }           // その点が壁の中か
//   // CSVに置いた物(敵・岩・スタート位置など)を出す
//   for (int i = 0; i < m_stage.GetSpawnCount(); i++) {
//       StageSpawn spawn = m_stage.GetSpawn(i);   // spawn.id、spawn.x、spawn.y
//   }
//
// CSVに書く数字
//   0      ：空(水)
//   1      ：壁
//   2〜5   ：45度の斜面(仮の番号。レベルデザインと相談して決める)
//   100以上：物の配置(そのマスは空になる。番号の意味はレベルデザインと決める)
//   -1     ：空(Tiled で CSV を書き出すと空のマスが -1 になるため)
//
// ステージの大きさ：一番引いた時(1.6倍)に画面がはみ出さないように、横48マス・縦27マス以上にする
//
#include "Object.h"
#include "Config.h"
#include "WallList.h"

#define STAGE_W_MAX		(320)	// ステージの横幅の最大(マス数)
#define STAGE_H_MAX		(96)	// ステージの縦幅の最大(マス数)
#define STAGE_SPAWN_MAX	(256)	// 物の配置の最大数
#define TILE_SHEET_X	(3)		// タイル画像の横のコマ数
#define TILE_SHEET_Y	(4)		// タイル画像の縦のコマ数

// タイルの番号
#define TILE_EMPTY		(0)		// 空(水)
#define TILE_SOLID		(1)		// 壁
#define TILE_SLOPE_BR	(2)		// 斜面 ◢ (右下が壁。左下から右上への坂)
#define TILE_SLOPE_BL	(3)		// 斜面 ◣ (左下が壁。左上から右下への坂)
#define TILE_SLOPE_TL	(4)		// 斜面 ◤ (左上が壁)
#define TILE_SLOPE_TR	(5)		// 斜面 ◥ (右上が壁)
#define TILE_MAX		(5)		// タイルの番号の最大
#define SPAWN_ID_MIN	(100)	// これ以上の番号は物の配置

// CSVに置いた物1つ
struct StageSpawn
{
	int id;		// 番号(100以上)
	float x;	// マスの中心の位置(ワールド座標)
	float y;
};

// Camera の前方宣言(ポインタだけ使うので、中身は .cpp で include する)
class Camera;

class Stage
{
public:
	void Init(const char* filename);	// ステージを初期化(タイル画像を読み込み、Load する)
	bool Load(const char* filename);	// CSVファイルを読み込み、壁の線を作る。失敗したら false
	void Draw(Camera* pCamera);		// ステージを描画(画面に映っているタイルだけ)
	void Uninit();					// ステージを終了

	WallList* GetWalls();			// 壁の線(Body::Move に渡す)
	bool IsSolid(float x, float y);	// その点(ワールド座標)が壁の中か
	float GetWidth();				// ステージの横幅(ドット)
	float GetHeight();				// ステージの縦幅(ドット)

	int GetSpawnCount();			// CSVに置いた物の数
	StageSpawn GetSpawn(int index);	// index番目の物

	// マスのタイルを変える(x:列、y:CSVの行(上から数える))。壁の線も作り直す(ボス部屋の入口を閉じる時など)
	// ※魚がいるマスを壁にすると、魚が閉じ込められる。魚が通り過ぎてから変えること
	void SetTile(int x, int y, int tile);

private:
	Object m_tile;	// タイル画像
	unsigned char m_data[STAGE_H_MAX][STAGE_W_MAX];	// ステージデータ(タイルの番号。0〜255)
	int m_width = 0;	// ステージの横幅(マス数)
	int m_height = 0;	// ステージの縦幅(マス数)
	WallList m_walls;	// 壁の線
	StageSpawn m_spawn[STAGE_SPAWN_MAX];	// CSVに置いた物
	int m_spawnCount = 0;					// CSVに置いた物の数

	bool LoadCSV(const char* filename);	// CSVファイルを読み込む
	void SetCell(int x, int y, int number);	// CSVの数字1つを、タイルか物の配置として入れる
	int GetTile(int x, int y);			// マスのタイルの番号(x:列、y:CSVの行(上から数える)。ステージの外は壁)
	void MakeWalls();					// タイルから壁の線を作る
};
