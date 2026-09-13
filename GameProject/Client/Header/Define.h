#pragma once

#include <vector>
#include <string>
#include "Engine_Define.h"

/* 벽 방향 */
enum class EWallDir
{
	NONE,
	EAST,
	SOUTH,
	WEST,
	NORTH,

	MAX,
};

/* 맵 정보를 담은 구조체 */
struct TRoomEntity
{
	int    iType = 0;
	wstring wstrEntityName;
	_vec3  vPos = _vec3{ 0.f, 0.f, 0.f };
};

struct TRoomData
{
	int iVersion = 0;
	wstring wstrRoomName;
	int iDefaultTileIdx;
	vector<int> vecTile;
	vector<int> vecObjectTilingInfo;
	vector<TRoomEntity> vecObjectInfo;
	vector<bool> vecDoorInfo;
	vector<int> vecDoorTile;
};

/* 맵 배치 오브젝트 타입 */
enum class EObjectType
{
	NONE = 0,

	BREAKABLE_FRUSTUM = 1,
	UNBREAKABLE_FRUSTUM,
	EXPLOSIVE_FRUSTUM,

	MONSTER = 11,

	MAX,
};