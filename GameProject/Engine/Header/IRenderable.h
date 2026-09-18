#pragma once

#include "Engine_Define.h"

class CBase;

BEGIN(Engine)

class IRenderable
{
public:
	virtual void Render(LPDIRECT3DDEVICE9& pGraphicDev) PURE;
	virtual _float Get_ViewZ() PURE;
	virtual CBase* GetBase() PURE;
};

END