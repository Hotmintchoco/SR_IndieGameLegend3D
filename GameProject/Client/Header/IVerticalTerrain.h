#pragma once

#include "Engine_Define.h"

class IVerticalTerrain
{
public:
	/* 2D(x/z 평면) 기준 단단한 영역 : vMin/vMax의 x = 월드 x, y = 월드 z
	   2D 플레이어의 착지 / 머리 / 옆면 판정에 모두 사용. 막지 않는 상태면 false */
	virtual bool GetFlatSolidBounds(_vec2& vMin, _vec2& vMax) PURE;
};

