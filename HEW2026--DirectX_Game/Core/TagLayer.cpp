#include "TagLayer.h"

// 当たるレイヤーの組み合わせ(Unity の Layer Collision Matrix)
// true:当たる、false:当たらない。[a][b] と [b][a] のどちらかが false なら当たらない(左右対称に書く)
static const bool g_layerHit[HIT_LAYER_MAX][HIT_LAYER_MAX] =
{
	//                   DEFAULT  PLAYER  ENEMY
	/* DEFAULT */	{ true,    true,   true  },
	/* PLAYER  */	{ true,    false,  true  },
	/* ENEMY   */	{ true,    true,   false },	// 敵同士は当たらない
};

bool IsLayerHit(HIT_LAYER a, HIT_LAYER b)
{
	return g_layerHit[a][b] && g_layerHit[b][a];
}
