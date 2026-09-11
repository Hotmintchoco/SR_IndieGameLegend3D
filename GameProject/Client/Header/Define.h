#pragma once

/* 벽 방향 */
enum class EWallDir
{
	NONE,
	EAST,
	WEST,
	SOUTH,
	NORTH,

	MAX,
};

/* 맵 정보를 담은 구조체 */
struct TMapEntity
{
	int    iType = 0;
	wstring wstrEntityName;
	_vec3  vPos = _vec3{ 0.f, 0.f, 0.f };
};

struct TMapData
{
	int iVersion = 0;
	wstring wstrMapName;
	vector<int> vecTile;
	vector<TMapEntity> vecObjectInfo;
	vector<TMapEntity> vecMonsterInfo;
	vector<bool> vecDoorInfo;
};