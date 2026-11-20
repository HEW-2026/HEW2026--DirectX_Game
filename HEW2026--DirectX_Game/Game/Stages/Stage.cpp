#include "Stage.h"
#include "Camera.h"
#include <stdio.h>	// ファイル読み込み
#include <math.h>

// タイルの辺
#define SIDE_LEFT	(0)	// 左の辺
#define SIDE_RIGHT	(1)	// 右の辺
#define SIDE_BOTTOM	(2)	// 下の辺
#define SIDE_TOP	(3)	// 上の辺

// タイルの辺が端から端まで壁か
static bool IsSideFull(int tile, int side)
{
	switch (tile) {
	case TILE_SOLID:	return true;
	case TILE_SLOPE_BR:	return side == SIDE_BOTTOM || side == SIDE_RIGHT;	// ◢
	case TILE_SLOPE_BL:	return side == SIDE_BOTTOM || side == SIDE_LEFT;	// ◣
	case TILE_SLOPE_TL:	return side == SIDE_TOP || side == SIDE_LEFT;		// ◤
	case TILE_SLOPE_TR:	return side == SIDE_TOP || side == SIDE_RIGHT;		// ◥
	}
	return false; // 空
}

void Stage::Init(const char* filename)
{
	// タイル画像を読み込む(横3分割、縦4分割)
	m_tile.Init("asset/Test_Tile.png", TILE_SHEET_X, TILE_SHEET_Y);

	// ステージデータを読み込み、壁の線を作る
	Load(filename);
}

bool Stage::Load(const char* filename)
{
	bool isOk = LoadCSV(filename);
	MakeWalls();
	return isOk;
}

void Stage::Draw(Camera* pCamera)
{
	// 画面に映るマスだけ描く(揺れで端が欠けないように1マス広め)
	int startX = (int)floorf(pCamera->GetViewLeft() / TILE_SIZE) - 1;
	int endX = (int)floorf(pCamera->GetViewRight() / TILE_SIZE) + 1;
	int startY = m_height - 1 - (int)floorf(pCamera->GetViewTop() / TILE_SIZE) - 1;	// CSVの行は上から数える
	int endY = m_height - 1 - (int)floorf(pCamera->GetViewBottom() / TILE_SIZE) + 1;
	if (startX < 0) startX = 0;
	if (startY < 0) startY = 0;
	if (endX > m_width - 1) endX = m_width - 1;
	if (endY > m_height - 1) endY = m_height - 1;

	for (int y = startY; y <= endY; y++) {
		for (int x = startX; x <= endX; x++) {
			int tile = m_data[y][x];
			// 空のマスは描画しない
			if (tile == TILE_EMPTY) continue;

			// 画像のコマ(番号1が左上。右へ進み、端で次の段)
			m_tile.numU = (float)((tile - 1) % TILE_SHEET_X);
			m_tile.numV = (float)((tile - 1) / TILE_SHEET_X);

			// マスの中心の座標(ワールド座標)
			float centerX = x * TILE_SIZE + TILE_SIZE / 2.0f;
			float centerY = (m_height - 1 - y) * TILE_SIZE + TILE_SIZE / 2.0f;
			pCamera->Draw(&m_tile, centerX, centerY, TILE_SIZE, TILE_SIZE, DRAW_LAYER_STAGE);
		}
	}
}

void Stage::Uninit()
{
	m_tile.Uninit(); // タイルを終了
}

WallList* Stage::GetWalls()
{
	return &m_walls;
}

bool Stage::IsSolid(float x, float y)
{
	// どのマスにあるか(row はCSVの行。上から数える)
	int col = (int)floorf(x / TILE_SIZE);
	int rowFromBottom = (int)floorf(y / TILE_SIZE);
	int row = m_height - 1 - rowFromBottom;
	int tile = GetTile(col, row);

	// マスの左下から見た位置(0〜TILE_SIZE)
	float localX = x - col * TILE_SIZE;
	float localY = y - rowFromBottom * TILE_SIZE;

	switch (tile) {
	case TILE_SOLID:	return true;
	case TILE_SLOPE_BR:	return localY < localX;					// ◢ 対角線より下
	case TILE_SLOPE_BL:	return localY < TILE_SIZE - localX;		// ◣
	case TILE_SLOPE_TL:	return localY > localX;					// ◤ 対角線より上
	case TILE_SLOPE_TR:	return localY > TILE_SIZE - localX;		// ◥
	}
	return false;
}

float Stage::GetWidth()
{
	return (float)(m_width * TILE_SIZE);
}

float Stage::GetHeight()
{
	return (float)(m_height * TILE_SIZE);
}

int Stage::GetSpawnCount()
{
	if (m_spawnCount > STAGE_SPAWN_MAX) return STAGE_SPAWN_MAX;
	return m_spawnCount;
}

StageSpawn Stage::GetSpawn(int index)
{
	return m_spawn[index];
}

void Stage::SetTile(int x, int y, int tile)
{
	if (x < 0 || x >= m_width || y < 0 || y >= m_height) return;
	if (tile < 0 || tile > TILE_MAX) tile = TILE_EMPTY; // 知らない番号は空にする
	m_data[y][x] = (unsigned char)tile;
	MakeWalls(); // 壁の線を作り直す
}

int Stage::GetTile(int x, int y)
{
	// ステージの外は壁にする(外に出られないように)
	if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
		return TILE_SOLID;
	}
	return m_data[y][x];
}

void Stage::MakeWalls()
{
	m_walls.Clear();

	// ステージの外側1マス(壁)も含めて調べる
	// x：列、y：CSVの行(y - 1 が上のマス)
	for (int y = -1; y <= m_height; y++) {
		for (int x = -1; x <= m_width; x++) {
			int tile = GetTile(x, y);
			if (tile == TILE_EMPTY) continue;

			// マスの四隅(ワールド座標)
			float left = (float)(x * TILE_SIZE);
			float right = left + TILE_SIZE;
			float bottom = (float)((m_height - 1 - y) * TILE_SIZE);
			float top = bottom + TILE_SIZE;

			// 隣が壁でない辺だけを壁の線にする
			if (IsSideFull(tile, SIDE_LEFT) && !IsSideFull(GetTile(x - 1, y), SIDE_RIGHT)) {
				m_walls.AddLine({ left, bottom }, { left, top });
			}
			if (IsSideFull(tile, SIDE_RIGHT) && !IsSideFull(GetTile(x + 1, y), SIDE_LEFT)) {
				m_walls.AddLine({ right, bottom }, { right, top });
			}
			if (IsSideFull(tile, SIDE_TOP) && !IsSideFull(GetTile(x, y - 1), SIDE_BOTTOM)) {
				m_walls.AddLine({ left, top }, { right, top });
			}
			if (IsSideFull(tile, SIDE_BOTTOM) && !IsSideFull(GetTile(x, y + 1), SIDE_TOP)) {
				m_walls.AddLine({ left, bottom }, { right, bottom });
			}

			// 斜面は対角線も壁の線にする
			if (tile == TILE_SLOPE_BR || tile == TILE_SLOPE_TL) {
				m_walls.AddLine({ left, bottom }, { right, top });	// 左下から右上
			}
			if (tile == TILE_SLOPE_BL || tile == TILE_SLOPE_TR) {
				m_walls.AddLine({ left, top }, { right, bottom });	// 左上から右下
			}
		}
	}
}

void Stage::SetCell(int x, int y, int number)
{
	// 配列の外には書かない
	if (x >= STAGE_W_MAX || y >= STAGE_H_MAX) return;

	// 100以上は物の配置(マスは空にする)
	if (number >= SPAWN_ID_MIN)
	{
		m_data[y][x] = TILE_EMPTY;
		if (m_spawnCount < STAGE_SPAWN_MAX)
		{
			m_spawn[m_spawnCount].id = number;
			m_spawn[m_spawnCount].x = x * TILE_SIZE + TILE_SIZE / 2.0f;
			m_spawn[m_spawnCount].y = (float)y;	// 仮にCSVの行を入れる(LoadCSV の最後でワールド座標に直す)
			m_spawnCount++;
		}
		else if (m_spawnCount == STAGE_SPAWN_MAX)
		{
			MessageBoxA(NULL, "物の配置が多すぎます。STAGE_SPAWN_MAXを増やしてください。", "エラー", MB_ICONERROR | MB_OK);
			m_spawnCount++; // 上限+1にしてメッセージを1回だけにする(GetSpawnCount は上限までを返す)
		}
		return;
	}

	// 知らない番号や -1 は空にする
	if (number < 0 || number > TILE_MAX)
	{
		number = TILE_EMPTY;
	}
	m_data[y][x] = (unsigned char)number;
}

bool Stage::LoadCSV(const char* filename)
{
	// ステージデータを初期化（一旦全部 TILE_EMPTY(水)にする）
	for (int y = 0; y < STAGE_H_MAX; y++)
	{
		for (int x = 0; x < STAGE_W_MAX; x++)
		{
			m_data[y][x] = TILE_EMPTY;
		}
	}
	m_width = 0;
	m_height = 0;
	m_spawnCount = 0;

	// CSVファイルを開く
	FILE* fp = nullptr;
	if (fopen_s(&fp, filename, "r") != 0)
	{
		// ファイルが開けなかった場合はエラー
		MessageBoxA(NULL, "ステージデータの読み込みに失敗しました。", "エラー", MB_ICONERROR | MB_OK);
		return false;
	}

	int cell_PosX = 0;		//今読んでいるマス（横方向）
	int cell_PosY = 0;		//今読んでいるマス（縦方向）
	int tile_ID = 0;		//今読んでいる数字（タイルの番号）
	bool isMinus = false;	//今読んでいる数字にマイナスが付いているか
	bool isCellNumber = false;	//今のマスに数字があったか
	bool isRow = false;		//今の行に何か書いてあるか（空の行を数えないため）

	// 1文字ずつ読み込む
	int c = fgetc(fp);
	while (c != EOF)
	{
		if (c >= '0' && c <= '9')
		{
			//数字の桁を1つ増やす
			tile_ID = tile_ID * 10 + (c - '0');
			isCellNumber = true;
			isRow = true;
		}
		else if (c == '-')
		{
			isMinus = true;
		}
		else if (c == ',' || c == '\n')
		{
			//行の最後のカンマの後ろ（「1,0,1,」の最後）は、マスとして数えない
			bool isLastEmpty = (c == '\n' && !isCellNumber && cell_PosX > 0);
			if (c == ',') isRow = true;

			//区切りなら数字をマスにいれる
			if (isRow && !isLastEmpty)
			{
				if (isMinus) tile_ID = -tile_ID;
				SetCell(cell_PosX, cell_PosY, tile_ID);
				cell_PosX++;	//次のマスに移動
			}
			tile_ID = 0;	//数字をリセット
			isMinus = false;
			isCellNumber = false;

			//改行なら次の行に移動
			if (c == '\n')
			{
				if (isRow)
				{
					// 一番長い行の長さを、ステージの横幅にする
					if (cell_PosX > m_width) m_width = cell_PosX;
					cell_PosY++;
				}
				cell_PosX = 0;
				isRow = false;
			}
		}
		c = fgetc(fp);
	}
	// 最後の数字をいれる（ファイルの最後に改行がない場合）
	if (isCellNumber)
	{
		if (isMinus) tile_ID = -tile_ID;
		SetCell(cell_PosX, cell_PosY, tile_ID);
		cell_PosX++;
	}
	if (isRow)
	{
		if (cell_PosX > m_width) m_width = cell_PosX;
		cell_PosY++;
	}
	fclose(fp); // ファイルを閉じる

	m_height = cell_PosY;

	// 大きすぎるステージは、入る分だけにする
	if (m_width > STAGE_W_MAX || m_height > STAGE_H_MAX)
	{
		MessageBoxA(NULL, "ステージが大きすぎます。STAGE_W_MAX、STAGE_H_MAXを増やしてください。", "エラー", MB_ICONERROR | MB_OK);
		if (m_width > STAGE_W_MAX) m_width = STAGE_W_MAX;
		if (m_height > STAGE_H_MAX) m_height = STAGE_H_MAX;
	}

	// 物の配置の y を、CSVの行からワールド座標に直す
	for (int i = 0; i < GetSpawnCount(); i++)
	{
		int row = (int)m_spawn[i].y;
		m_spawn[i].y = (m_height - 1 - row) * TILE_SIZE + TILE_SIZE / 2.0f;
	}
	return true;
}
