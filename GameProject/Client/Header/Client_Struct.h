#pragma once

#include <vector>
#include <string>
#include "Engine_Define.h"
#include "Client_Enum.h"

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
	vector<wstring> vecClearCondition;
};

struct TRoomEventCtx
{
	ERoomEventType eType;

};