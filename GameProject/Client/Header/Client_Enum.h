#pragma once

/* �� ���� */
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

enum COLLISIONID
{
	/* COLL_ID1  */	COLL_PLAYER,
	/* COLL_ID2  */	COLL_MONSTER,
	/* COLL_ID3  */	COLL_ID3,
	/* COLL_ID4  */	COLL_MBULLET,
	/* COLL_ID5  */	COLL_ID5,
	/* COLL_ID6  */	COLL_ITEM,
	/* COLL_ID7  */	COLL_OBSTACLE,
	/* COLL_ID8  */	COLL_EXPLODE,
	/* COLL_ID9  */	COLL_ID9,
	/* COLL_ID10 */	COLL_ID10,
	/* COLL_ID11 */	COLL_PROJECTILE,
	/* COLL_ID12 */	COLL_ROOMLOGIC,
	/* COLL_ID13 */	COLL_ID13,
	/* COLL_ID14 */	COLL_ID14,
	/* COLL_ID15 */	COLL_ID15,
	/* COLL_ID16 */	COLL_ID16,
	/* COLL_ID17 */	COLL_ID17,
	/* COLL_ID18 */	COLL_ID18,
	/* COLL_ID19 */	COLL_ID19,
	/* COLL_ID20 */	COLL_ID20,
	/* COLL_ID21 */	COLL_ID21,
	/* COLL_ID22 */	COLL_ID22,
	/* COLL_ID23 */	COLL_ID23,
	/* COLL_ID24 */	COLL_ID24,
	/* COLL_ID25 */	COLL_ID25,
	/* COLL_ID26 */	COLL_ID26,
	/* COLL_ID27 */	COLL_ID27,
	/* COLL_ID28 */	COLL_ID28,
	/* COLL_ID29 */	COLL_ID29,
	/* COLL_ID30 */	COLL_ID30,
	/* COLL_ID31 */	COLL_ID31,
	/* COLL_ID32 */	COLL_ID32,

	COLL_MAX
};

/* �� ��ġ ������Ʈ Ÿ�� */
enum class EObjectType
{
	NONE = 0,

	BREAKABLE_FRUSTUM = 1,
	UNBREAKABLE_FRUSTUM,
	EXPLOSIVE_FRUSTUM,
	GAME_MACHINE,
	DDOKDDAK,
	LIMINAL_CUBE,
	LIMINAL_SLOPE,

	Skull = 11,
	Boss1 = 12,
	Speyeder = 13,
	Magmamouth = 14,
	Worm = 15,
	Sprnub1 = 16,
	Sprnub2 = 17,
	Sprnub3 = 18,
	Cryder = 19,
	Glubba = 20,


	ITEM_NONE = 31,
	ITEM_HEART,
	ITEM_GEM,
	ITEM_ENERGY,
	ITEM_MAX,

	WEAPON_NONE = 41,
	WEAPON_DEFAULT,
	WEAPON_SHOTGUN,
	WEAPON_LASERGUN,
	WEAPON_BOW,
	WEAPON_LIMINAL,
	WEAPON_MAX,


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

enum class EDirection
{
	NONE,

	EAST,	// +x
	SOUTH,	// -z
	WEST,	// -x
	NORTH,	// +z

	MAX,
};

enum class EBiomeType
{
	NONE,

	CYBER,
	DESERT,
	AQUA,
	SNOW,
	LAVA,

	MAX,
};

enum PLAYERPART { PP_BODY, PP_HEAD, PP_LARM, PP_RARM, PP_LLEG, PP_RLEG, PP_END };

enum class EPlayerLocomotionState
{
	NONE,

	IDLE,
	WALK,
	SPRINT,
	JUMP,

	MAX,
};

enum class EPlayerActionState
{
	NONE,

	GUN_SHOOT,
	BOW_HOLD,
	BOW_SHOOT,
	DIE,

	MAX,
};

enum class EWeaponAction
{
	Primary,
	Secondary,

	MAX,
};

enum class EInputState
{
	NONE,

	Pressed,
	Held,
	Released,

	MAX,
};

enum class EWeaponEvent
{
	NONE,

	GUN_SHOT,
	BOW_CHARGE_START,
	BOW_CHARGE_END,

	MAX,
};

enum class EColorTexture
{
	NONE,

	BLACK,
	ORANGE,
	PINK,
	RED,
	YELLOW,
	WHITE,
	SAND,

	MAX,
};