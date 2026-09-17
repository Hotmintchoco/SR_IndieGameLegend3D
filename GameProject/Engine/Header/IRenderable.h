#pragma once

#include "Engine_Define.h"

BEGIN(Engine)

class IRenderable
{
public:
	virtual void Render(LPDIRECT3DDEVICE9& pGraphicDev) PURE;
	virtual _float Get_ViewZ() PURE;
};

END