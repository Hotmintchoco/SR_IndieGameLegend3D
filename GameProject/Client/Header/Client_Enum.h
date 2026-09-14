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