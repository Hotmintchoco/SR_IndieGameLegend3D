#pragma once

#include "Engine_Define.h"

namespace Engine
{
	class CVIBuffer;
	class CTransform;
}

class IAnamorphicTarget
{
public:
	virtual CVIBuffer* GetBuffer() PURE;
	virtual CTransform* GetTransform() PURE;
};

