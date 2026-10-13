#pragma once

#include "Engine_Define.h"

class IVerticalTerrain
{
public:
	virtual float SampleTerrainHeight(const _vec3& vRayStart) PURE;
};

