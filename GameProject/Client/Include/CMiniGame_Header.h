#ifndef MiniGame_Header_h_
#define MiniGame_Header_h_

#include "Engine_Define.h"

/* CMiniGame */
#define MG_MAX_LIFE				5
#define MG_HEART_FULL			4
#define MG_START_INVINCIBLE		1.6f
#define MG_HIT_INVINCIBLE		2.f
#define MG_HIT_FLASH_TIME		0.4f
#define MG_GAMEOVER_DELAY		3.f
#define MG_FVF					(D3DFVF_XYZ | D3DFVF_DIFFUSE)
#define MG_BGM_START_TIME		3.f			
#define MG_BGM_LENGTH			146.57f		
#define MG_CLEAR_DELAY			3.f			
#define MG_CLEAR_SHOW_TIME		3.f

#define MG_FIELD_MIN_X			(-10.f)
#define MG_FIELD_MAX_X			10.f
#define MG_FIELD_MIN_Y			(-1.5f)
#define MG_FIELD_MAX_Y			7.f
#define MG_SPAWN_Z				115.f
#define MG_DESPAWN_Z			(-15.f)

#define MG_FOG_START			45.f
#define MG_FOG_END				110.f
#define MG_FOG_COLOR			D3DCOLOR_ARGB(255, 16, 24, 48)

/* CMG0_Deck, CMG0_Card */
#define MG_BASE_ATTACK			100
#define MG_CARD_HP				20
#define MG_CARD_SKIP_HP			2
#define MG_CARD_HAND_COUNT		3
#define MG_CARD_DESC_LINE		3
#define MG_CARD_MAX_TURN		10
#define MG_CARD_START_TIME		(MG_BGM_START_TIME + 23.2f)
#define MG_CARD_END_TIME		(MG_BGM_START_TIME + 77.f)
#define MG_CARD_READY_TIME		2.7f
#define MG_CARD_COMPLETE_TIME	3.f
#define MG_CARD_NOTICE_TIME		1.5f
#define MG_CARD_SELECT_NONE		(-1)
#define MG_CARD_SELECT_REST		MG_CARD_HAND_COUNT
#define MG_CARD_UI_HALF_X		60.f
#define MG_CARD_UI_HALF_Y		60.f
#define MG_CARD_UI_GAP_X		150.f
#define MG_CARD_UI_CENTER_Y		500.f
#define MG_CARD_UI_SELECT_RAISE	16.f
#define MG_MOOD_GOOD_RATE		1.5f
#define MG_MOOD_PERFECT_RATE	0.1f

/* CMG0_Player */
#define MG_PLAYER_CENTER		_vec3(0.f, 2.f, 0.f)

/* CMG0_HUD */
#define MG_UI_TEXT_HEIGHT		22
#define MG_UI_NUM_HEIGHT		18
#define MG_UI_FONT_SCALE		2
#define MG_UI_LINE_HEIGHT		24.f
#define MG_UI_ICON_SIZE			30.f
#define MG_UI_COUNT_SPEED		30.f
#define MG_UI_SELECT_SIZE_X		84.f
#define MG_UI_SELECT_SIZE_Y		54.f
#define MG_UI_WEAPON_SIZE		48.f
#define MG_WEAPON_STONE			200
#define MG_WEAPON_IRON			500
#define MG_WEAPON_DIAMOND		800
#define MG_GUIDE_DELAY			0.4f
#define MG_GUIDE_END_TIME		(MG_BGM_START_TIME + 16.5f)
#define MG_TUTORIAL_BOX_HALF_X	240.f
#define MG_TUTORIAL_BOX_HALF_Y	62.f

/* CMG0_Intro */
#define MG_INTRO_WHITE_TIME		0.4f
#define MG_INTRO_SHRINK_Y_TIME	0.35f
#define MG_INTRO_SHRINK_X_TIME	0.25f
#define MG_INTRO_BLACK_TIME		0.2f
#define MG_INTRO_FADE_TIME		1.5f
#define MG_INTRO_TOTAL_TIME		(MG_INTRO_WHITE_TIME + MG_INTRO_SHRINK_Y_TIME + MG_INTRO_SHRINK_X_TIME + MG_INTRO_BLACK_TIME + MG_INTRO_FADE_TIME)

/* CMG0_Dialogue */
#define MG_DIALOGUE_START_TIME		(MG_INTRO_TOTAL_TIME - MG_INTRO_FADE_TIME)
#define MG_DIALOGUE_END_TIME		(MG_BGM_START_TIME + 12.3f)
#define MG_DIALOGUE_WORD_TIME		0.25f
#define MG_DIALOGUE_PORTRAIT_TIME	0.36f
#define MG_DIALOGUE_FONT_SIZE		32.f
#define MG_DIALOGUE_WINDOW_Y		157.f
#define MG_DIALOGUE_WINDOW_HALF_X	300.f
#define MG_DIALOGUE_WINDOW_HALF_Y	53.f
#define MG_DIALOGUE_PORTRAIT_X		153.f
#define MG_DIALOGUE_PORTRAIT_HALF	46.f
#define MG_DIALOGUE_TEXT_X			231.f
#define MG_DIALOGUE_GUIDE_Y			238.f
#define MG_DIALOGUE_GUIDE_HALF_X	150.f
#define MG_DIALOGUE_GUIDE_HALF_Y	20.f
#define MG_DIALOGUE_GUIDE			L"ENTER TO CONTINUE"
#define MG_DIALOGUE_WINDOW_PATH		L"../Bin/Resource/Texture/Intro/ChatWindow.png"
#define MG_DIALOGUE_PORTRAIT_PATH	L"../Bin/Resource/Texture/Intro/bean_%d.png"

/* CMG0_Dancer */
#define MG_DANCER_POS			_vec3(0.f, 4.95f, 3.24f)
#define MG_DANCER_SCALE			3.f
#define MG_DANCER_ENTER_HEIGHT	9.f
#define MG_DANCER_ENTER_TIME	3.f

/* CMG0_Sunglass */
#define MG_SUNGLASS_PATH		L"../Bin/Resource/Texture/MiniGame_0/SwagSunglass.png"
#define MG_SUNGLASS_POS			_vec3(0.f, 0.135f, 0.16f)
#define MG_SUNGLASS_HALF_X		0.19f
#define MG_SUNGLASS_HALF_Y		0.123f
#define MG_SUNGLASS_DROP_HEIGHT	0.6f
#define MG_SUNGLASS_DROP_TIME	0.6f
#define MG_SUNGLASS_SFX_VOLUME	3.f

/* CMG0_Dancer Pilot */
#define MG_PILOT_SCALE			1.f
#define MG_PILOT_OFFSET			_vec3(0.f, -0.35f, -0.3f)
#define MG_PILOT_HEIGHT			8.5f
#define MG_PILOT_BOARD_TIME		2.5f
#define MG_PILOT_LEG_ANGLE		-90.f
#define MG_PILOT_ARM_ANGLE		-40.f

#define MG_DANCE_BEAT			0.5f
#define MG_DANCE_MOVE_RATE		0.6f
#define MG_DANCE_POSE_COUNT		4

/* CMG0_Train13 */
#define MG_LANE_COUNT			5
#define MG_LANE_GAP				(MGBG_TILE_SIZE * 0.5f)
#define MG_LANE_MOVE_SPEED		22.f
#define MG_LANE_REPEAT_DELAY	0.12f
#define MG_LANE_REPEAT_TIME		0.07f
#define MG_TRAIN_GROUND_Y		(-4.f)
#define MG_TRAIN_SCALE			2.52f
#define MG_TRAIN_HIT_X			1.9f
#define MG_TRAIN_HIT_Z			4.1f
#define MG_TRAIN_SPEED_MIN		60.f
#define MG_TRAIN_SPEED_MAX		100.f
#define MG_TRAIN_SPAWN_MAX		0.75f
#define MG_TRAIN_SPAWN_MIN		0.35f
#define MG_TRAIN_RAMP_TIME		25.f
#define MG_TRAIN_START_DELAY	1.f
#define MG_TRAIN_END_TIME		(MG_BOSS_WARNING_START - 1.5f)
#define MG_TRAIN_CLEAR_TIME		3.5f
#define MG_TRAIN_PLAYER_Y		(-3.2f)
#define MG_TRAIN_CAM_TIME		1.f
#define MG_TRAIN_CAM_EYE_Y		7.f
#define MG_TRAIN_CAM_EYE_Z		(-7.5f)
#define MG_TRAIN_CAM_AT_Y		(-1.5f)
#define MG_TRAIN_CAM_AT_Z		9.f
#define MG_TRAIN_CAM_FOV		50.f
#define MG_CAM_AT_Z				14.f
#define MG_CAM_FOV				60.f

/* CMG0_Train14 (레인 2칸) */
#define MG_TRAIN14_SCALE		4.7f
#define MG_TRAIN14_HIT_X		(MG_LANE_GAP * 0.5f + MG_TRAIN_HIT_X)
#define MG_TRAIN14_HIT_Z		7.1f
#define MG_TRAIN14_CRASH_X		2.f
#define MG_TRAIN14_CRASH_Z		9.4f
#define MG_TRAIN14_WARN_TIME	1.f
#define MG_TRAIN14_START_RATE	0.2f
#define MG_TRAIN14_SPEED_RATE	1.5f
#define MG_TRAIN14_INTERVAL_MAX	6.f
#define MG_TRAIN14_INTERVAL_MIN	3.f
#define MG_TRAIN14_SAFE_WINDOW	0.3f

/* CMG0_Herobrine */
#define MG_BOSS_HP				50000
#define MG_BOSS_POS				_vec3(0.f, -4.f, 30.f)
#define MG_BOSS_SCALE			9.f
#define MG_BOSS_ENTER_HEIGHT	30.f
#define MG_BOSS_ENTER_TIME		3.f
#define MG_BOSS_FLOAT_HEIGHT	0.4f
#define MG_BOSS_FLOAT_SPEED		1.5f
#define MG_BOSS_WARNING_START	(MG_BOSS_ENTER_START - MG_BOSS_WARNING_TIME)
#define MG_BOSS_WARNING_TIME	3.f
#define MG_BOSS_WARNING_BLINK	0.3f
#define MG_WARNING_ALPHA_MIN	40.f
#define MG_WARNING_ALPHA_MAX	150.f
#define MG_BOSS_ENTER_START		(MG_BGM_START_TIME + 110.f)
#define MG_BOSS_HPBAR_START		MG_BOSS_ENTER_START
#define MG_BOSS_LIMIT_TIME		(MG_BGM_START_TIME + 136.f)
#define MG_BOSS_HIT_X			3.2f
#define MG_BOSS_HIT_Y			10.8f
#define MG_BOSS_HIT_Z			1.5f
#define MG_BOSS_CORE_Y			6.5f
#define MG_BOSS_PATTERN_REST	0.7f
#define MG_BOSS_SKULL_HP		2000
#define MG_BOSS_BULLET_SPEED	30.f
#define MG_BOSS_LASER_RADIUS	3.2f
#define MG_BOSS_LASER_WARN_TIME	1.f
#define MG_BOSS_LASER_FIRE_TIME	1.5f
#define MG_BOSS_SIDE_LASER_X	6.5f
#define MG_BOSS_SIDE_LASER_RADIUS	2.6f
#define MG_BOSS_DEAD_TIME		1.2f
#define MG_BOSS_EXPLODE_TIME	2.f
#define MG_BOSS_EXPLODE_GAP		0.1f
#define MG_WHITE_FADE_TIME		1.f
#define MG_WHITE_EXIT_TIME		3.f
#define MG_BOSS_TEX_PATH		L"../Bin/Resource/Mesh/Player.png"
#define MG_BOSS_FACE_PATH		L"../Bin/Resource/Texture/MiniGame_0/Herobrine_Face.png"

/* CMiniGameBG */
#define MGBG_TILE_SIZE			6.f
#define MGBG_TILE_OFFSET_X		(-1.3125f)
#define MGBG_TILE_FRAME			50
#define MGBG_TRAIN_TILE_FRAME	52
#define MGBG_BOSS_TILE_FRAME	47
#define MGBG_SCROLL_SPEED		30.f
#define MGBG_TILE_MIN_X			(-6)
#define MGBG_TILE_MAX_X			6
#define MGBG_TILE_MIN_Z			(-3)
#define MGBG_TILE_MAX_Z			19

namespace MINIGAME0
{
	enum WINMODE { MODE_FULL, MODE_WIN };

	enum COMPONENTID { ID_DYNAMIC, ID_STATIC, ID_END };

	enum INFO { INFO_RIGHT, INFO_UP, INFO_LOOK, INFO_POS, INFO_END };

	enum ROTATION { ROT_X, ROT_Y, ROT_Z, ROT_END };

	enum TEXTUREID { TEX_NORMAL, TEX_CUBE, TEX_END };

	enum RENDERID { RENDER_PRIORITY, RENDER_NONALPHA, RENDER_ALPHATEST, RENDER_ALPHA, RENDER_OVERLAY, RENDER_UI, RENDER_DEBUG_COLLIDER, RENDER_END };

	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };

	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

	enum MESHRENDERMODE { MESH_SOLID, MESH_WIREFRAME, MESH_NONE, MESH_MODE_END };

	enum BULLETTYPE { BULLET_PLAYER, BULLET_ENEMY, BULLET_END };

	enum MGCARD
	{
		CARD_APPEAL,
		CARD_BULB,
		CARD_TVAPPEAR,
		CARD_HIGHTOUCH,
		CARD_BREATH,
		CARD_EXCITE,
		CARD_SPEAKER,
		CARD_CALLRESPONSE,
		CARD_NATIONALIDOL,
		CARD_ACHIEVE,
		CARD_ENTERTAINMENT,
		CARD_PRESENCE,
		CARD_SPRECHCHOR,
		CARD_END
	};

	enum MGCARDTYPE { REUSABLE, ONCE_PER_LESSON };

	enum MGICON {	ICON_HP, ICON_STAMINA,
					ICON_MOOD_GOOD, ICON_MOOD_PERFECT, ICON_ATTENTION, ICON_SKILL_DOUBLE, ICON_COST_HALF, ICON_EXTRA_ACTION,
					ICON_SELECT,
					ICON_WEAPON_WOOD, ICON_WEAPON_STONE, ICON_WEAPON_IRON, ICON_WEAPON_DIAMOND, ICON_END };

	enum MGSTATUS { MOOD_GOOD, MOOD_PERFECT, ATTENTION, STAMINA, CARD_HP, COST_HALF, STATUS_END };

	struct MGCARDSTATE
	{
		_int	iStatus[STATUS_END]{};
		_bool	bFreshTurn[STATUS_END]{};
		_int	iExtraActivate = 0;
		_int	iExtraAction = 0;
		_int	iActiveParameter = 0;
		_int	iAttack = MG_BASE_ATTACK;
	};

	struct MGVTX
	{
		_vec3	vPos;
		DWORD	dwColor;
	};

	struct MGCARDLINE
	{
		MGICON			eIcon;
		const _tchar*	pText;
	};

	struct MGCARDDATA
	{
		const _tchar*	pName;
		MGCARDLINE		tLine[MG_CARD_DESC_LINE];
		MGCARDTYPE		eType;
	};

	struct MGICONGUIDE
	{
		MGICON			eIcon;
		const _tchar*	pName;
		const _tchar*	pDesc;
	};

	struct MGTUTORIAL
	{
		_float			fX, fY, fHalfX, fHalfY;
		_float			fBoxX, fBoxY;
		const _tchar*	pTitle;
		const _tchar*	pLine[2];
	};

	struct MGDANCEBEAT
	{
		_bool	bDance = false;
		_bool	bSit = false;
		_int	iPoseFrom = 0;
		_int	iPoseTo = 0;
		_float	fRate = 0.f;
	};

	struct MGCARDCOST
	{
		_int	iCardHp = 0;
		_int	iMoodGood = 0;
		_int	iAttention = 0;
	};

	static const MGCARDDATA MG_CARD_DATA[CARD_END] =
	{
		{ L"국힙의 기본",			{ { ICON_END, L"공격력 +14" },					{ ICON_END, L"" },									{ ICON_END, L"" } },							REUSABLE },
		{ L"영감의 기본",			{ { ICON_STAMINA, L"방어도 +7" },				{ ICON_END, L"" },									{ ICON_END, L"" } },							ONCE_PER_LESSON },
		{ L"쇼미 출연",				{ { ICON_STAMINA, L"방어도 +5" },				{ ICON_COST_HALF, L"5턴간 체력 소모 절반" },			{ ICON_END, L"" } },							ONCE_PER_LESSON },
		{ L"하이터치",				{ { ICON_END, L"공격력 +23 (날카로움 2배 적용)" },{ ICON_END, L"" },									{ ICON_END, L"" } },							ONCE_PER_LESSON },
		{ L"한숨",					{ { ICON_MOOD_GOOD, L"장인정신 4턴" },			{ ICON_ATTENTION, L"날카로움 +5" },					{ ICON_END, L"" } },							ONCE_PER_LESSON },
		{ L"샤우팅",					{ { ICON_END, L"공격력 +10" },					{ ICON_MOOD_PERFECT, L"헤파이스토스 정신 4턴"},		{ ICON_END, L"" } },							ONCE_PER_LESSON },
		{ L"하울링",					{ { ICON_MOOD_GOOD, L"장인정신 7턴" },			{ ICON_END, L"" },									{ ICON_END, L"" } },							ONCE_PER_LESSON },
		{ L"콜&리스폰스",			{ { ICON_END, L"공격력 +15" },					{ ICON_ATTENTION, L"날카로움 3 이상일때 공격력 +34" },{ ICON_END, L"" } },							ONCE_PER_LESSON },
		{ L"국힙 원탑",				{ { ICON_STAMINA, L"방어도 +8" },				{ ICON_SKILL_DOUBLE, L"다음 카드 2회 발동" },			{ ICON_EXTRA_ACTION, L"스킬카드 사용 수 +1" } },	ONCE_PER_LESSON },
		{ L"성취",					{ { ICON_MOOD_GOOD, L"장인정신 7턴" },			{ ICON_END, L"공격력 +40" },							{ ICON_END, L"" } },							ONCE_PER_LESSON },
		{ L"기획사 입사",			{ { ICON_END, L"액티브 카드 사용 시 공격력 +5" },	{ ICON_END, L"" },									{ ICON_END, L"" } },							ONCE_PER_LESSON },
		{ L"존재감",					{ { ICON_ATTENTION, L"날카로움 +6" },			{ ICON_EXTRA_ACTION, L"스킬카드 사용 수 +1" },		{ ICON_END, L"" } },							REUSABLE },
		{ L"떼창",					{ { ICON_END, L"공격력 +6" },					{ ICON_MOOD_GOOD, L"장인정신 3턴" },					{ ICON_EXTRA_ACTION, L"스킬카드 사용 수 +1" } },	REUSABLE },
	};

	static const MGICONGUIDE MG_ICON_GUIDE[] =
	{
		{ ICON_HP,				L"체력",				L"스킬카드 사용 시 소모, 휴식(V)으로 2 회복" },
		{ ICON_STAMINA,			L"방어도",				L"체력보다 먼저 소모됨" },
		{ ICON_MOOD_GOOD,		L"장인정신",			L"남은 턴 동안 공격력 상승량 1.5배" },
		{ ICON_MOOD_PERFECT,	L"헤파이스토스 정신",	L"장인정신 1턴당 상승 배율 +0.1" },
		{ ICON_ATTENTION,		L"날카로움",			L"공격력 상승 시 수치만큼 추가 상승" },
		{ ICON_SKILL_DOUBLE,	L"2회 발동",			L"다음에 사용하는 카드 효과 2회 발동" },
		{ ICON_COST_HALF,		L"체력 소모 절반",		L"남은 턴 동안 체력 소모 절반" },
		{ ICON_EXTRA_ACTION,	L"추가 행동",			L"이번 턴에 스킬카드 1장 더 사용 가능" },
	};

	static const MGTUTORIAL MG_TUTORIAL[] =
	{
		{ 400.f, 38.f, 120.f, 28.f,		400.f, 150.f,	L"턴",				{ L"카드게임은 최대 10턴까지 진행",		L"하단바가 끝나면 카드게임 종료" } },
		{ 400.f, 487.f, 220.f, 83.f,	400.f, 320.f,	L"카드 사용",		{ L"Z · X · C 로 카드 선택",				L"ENTER로 카드 사용" } },
		{ 700.f, 505.f, 72.f, 44.f,		490.f, 380.f,	L"휴식",				{ L"V를 누르고 ENTER로 휴식",					L"체력을 2 회복하고 턴 종료" } },
		{ 696.f, 141.f, 100.f, 47.f,	350.f, 141.f,	L"체력 · 방어도",	{ L"카드를 쓰면 정해진 수치만큼 체력 감소",				L"방어도가 있으면 방어도가 먼저 소모" } },
		{ 696.f, 56.f, 100.f, 44.f,		350.f, 80.f,	L"공격력",			{ L"카드로 올린 공격력만큼 플레이어 공격력 증가",	L"200 · 500 · 800 이상이면 무기 강화" } },
	};

	static const _tchar* MG_CARD_IMG_PATH[CARD_END] =
	{
		L"../Bin/Resource/Texture/MiniGame_0/Card_IMG/CARD_APPEAL.png",
		L"../Bin/Resource/Texture/MiniGame_0/Card_IMG/CARD_BULB.png",
		L"../Bin/Resource/Texture/MiniGame_0/Card_IMG/CARD_TVAPPEAR.png",
		L"../Bin/Resource/Texture/MiniGame_0/Card_IMG/CARD_HIGHTOUCH.png",
		L"../Bin/Resource/Texture/MiniGame_0/Card_IMG/CARD_BREATH.png",
		L"../Bin/Resource/Texture/MiniGame_0/Card_IMG/CARD_EXCITE.png",
		L"../Bin/Resource/Texture/MiniGame_0/Card_IMG/CARD_SPEAKER.png",
		L"../Bin/Resource/Texture/MiniGame_0/Card_IMG/CARD_CALLRESPONSE.png",
		L"../Bin/Resource/Texture/MiniGame_0/Card_IMG/CARD_NATIONALIDOL.png",
		L"../Bin/Resource/Texture/MiniGame_0/Card_IMG/CARD_ACHIEVE.png",
		L"../Bin/Resource/Texture/MiniGame_0/Card_IMG/CARD_ENTERTAINMENT.png",
		L"../Bin/Resource/Texture/MiniGame_0/Card_IMG/CARD_PRESENCE.png",
		L"../Bin/Resource/Texture/MiniGame_0/Card_IMG/CARD_SPRECHCHOR.png",
	};

	static const _tchar* MG_ICON_PATH[ICON_END] =
	{
		L"../Bin/Resource/Texture/MiniGame_0/Card_Icon/Icon_HP.png",
		L"../Bin/Resource/Texture/MiniGame_0/Card_Icon/Icon_Stamina.png",
		L"../Bin/Resource/Texture/MiniGame_0/Card_Icon/Icon_MoodGood.png",
		L"../Bin/Resource/Texture/MiniGame_0/Card_Icon/Icon_MoodPerfect.png",
		L"../Bin/Resource/Texture/MiniGame_0/Card_Icon/Icon_Attention.png",
		L"../Bin/Resource/Texture/MiniGame_0/Card_Icon/Icon_SkillDouble.png",
		L"../Bin/Resource/Texture/MiniGame_0/Card_Icon/Icon_CostHalf.png",
		L"../Bin/Resource/Texture/MiniGame_0/Card_Icon/Icon_ExtraAction.png",
		L"../Bin/Resource/Texture/MiniGame_0/Card_Icon/Icon_Select.png",
		L"../Bin/Resource/Texture/MiniGame_0/Weapon_IMG/Weapon_Wood.png",
		L"../Bin/Resource/Texture/MiniGame_0/Weapon_IMG/Weapon_Stone.png",
		L"../Bin/Resource/Texture/MiniGame_0/Weapon_IMG/Weapon_Iron.png",
		L"../Bin/Resource/Texture/MiniGame_0/Weapon_IMG/Weapon_Diamond.png",
	};

	static const _tchar* MG_DIALOGUE_TEXT[] =
	{
		L"[̵지̸금̷껏̴ ̵만̴난̸ ̵적̸ ̴없̵는̵\n강̸적̸을̸ ̴만̷나̵게̴ ̶될̷ ̶것̶]",
		L"인̸간..̶.̴\n무̵기̷... ̸나̴약̷하̴다̵.̸..",
		L"만약̸ ̸랩 ̴배̸틀̸을̶..̸.이̶긴다̷면...\n무̷기를̴..̸.̶강̷화̴해주̴지...̷",
	};

	static const MGCARD MG_DECK[] =
	{
		CARD_APPEAL,
		CARD_BULB, CARD_TVAPPEAR, CARD_HIGHTOUCH, CARD_BREATH, CARD_EXCITE, CARD_SPEAKER,
		CARD_CALLRESPONSE, CARD_NATIONALIDOL, CARD_ACHIEVE, CARD_ENTERTAINMENT,
		CARD_PRESENCE, CARD_SPRECHCHOR,
	};

	static const POINT MG_BOSS_EYE_PIXEL[] = { { 9, 12 }, { 10, 12 }, { 13, 12 }, { 14, 12 } };

	inline _float Random(_float fMin, _float fMax)
	{
		return fMin + (fMax - fMin) * (rand() / (_float)RAND_MAX);
	}

	inline _float Get_LaneX(_int iLane)
	{
		return (iLane - (MG_LANE_COUNT - 1) * 0.5f) * MG_LANE_GAP;
	}
}
#endif // MiniGame_Header_h_
