#pragma once

#include <vector>
#include <string>
#include <variant>
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
	int iBiome;
	bool bBossRoom;
	vector<int> vecTile;
	vector<int> vecObjectTilingInfo;
	vector<int> vecResistContamination;
	vector<TRoomEntity> vecObjectInfo;
	vector<bool> vecDoorInfo;
	vector<int> vecDoorTile;
	vector<wstring> vecClearCondition;
	bool bDark;
	int iClearReward;
};

struct TRoomEventCtx
{
	ERoomEventType eType;
	variant<int, bool> varArgs;
};

struct TTileIdx
{
	int iRow;
	int iCol;
};

struct TWeaponAnimArgs
{
	bool bSprint;
	bool bMove;
	bool bSpecialAtk;
};

struct TBiomeInfo
{
	EBiomeType eType;
	int iDefaultTileIndex;
};

struct TWeaponInput
{
	EWeaponAction eAction = EWeaponAction::MAX;
	EInputState eState = EInputState::NONE;
};

struct TWeaponOutput
{
	bool bAttackExecuted = false;	
	EWeaponAnimEvent eWpEvent = EWeaponAnimEvent::NONE;
};

struct TWeaponSystemInput
{
	/* 공격 관련 */
	TWeaponInput tWeaponInput[(int)EWeaponAction::MAX];
	bool bUltAttack = false;
	bool bSpecialSwitchPressed = false;
	bool bSwitchWeapon = false;

	/* 무기 애니메이션 관련 */
	bool bMove = false;
	bool bSprint = false;
};

struct TWeaponSystemOutput
{
	EWeaponAnimEvent eWpEvent = EWeaponAnimEvent::NONE;
};