#pragma once

#include <vector>

namespace Engine
{
	class CVIBuffer;
	class CTransform;
}

class IRayTestable
{
public:
	virtual vector<pair<Engine::CVIBuffer*, Engine::CTransform*>> GetRayTestTargetInfo() PURE;
};

