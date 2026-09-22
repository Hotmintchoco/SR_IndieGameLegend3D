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

enum JUMPSTATE { JUMP_NOT, JUMP_PARABOLIC, JUMP_FREEFALL, JUMP_END };

enum BULLETID { BULLET_DEFAULT, BULLET_SMALL, BULLET_END };


/* 맵 배치 오브젝트 타입 */
enum class EObjectType
{
	NONE = 0,

	BREAKABLE_FRUSTUM = 1,
	UNBREAKABLE_FRUSTUM,
	EXPLOSIVE_FRUSTUM,
	GAME_MACHINE,

	Skull = 11,
	Boss1,
	Speyeder,
	Magmamouth,
	Worm,

	ITEM_NONE = 31,
	ITEM_HEART,
	ITEM_GEM,
	ITEM_ENERGY,
	ITEM_MAX,

	MAX,
};

enum class ERoomEventType
{
	NONE,

	ROOM_CHANGED,
	ROOM_BEGIN,
	ROOM_CLEAR,
	
	RESET_ROOM,

	BUTTON,

	MAX,
};

enum ITEMID { ITEM_HEAL, ITEM_SKILLGAUGE, ITEM_END };

enum class EContaminateType
{
	NONE,

	LAVA,

	MAX,
};

enum class ETileType
{
	NONE,

	SPRITE,
	BUTTON,

	MAX,
};