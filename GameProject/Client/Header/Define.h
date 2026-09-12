#pragma once

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
	vector<TRoomEntity> vecObjectInfo;
	vector<TRoomEntity> vecMonsterInfo;
	vector<bool> vecDoorInfo;
	vector<int> vecDoorTile;
};