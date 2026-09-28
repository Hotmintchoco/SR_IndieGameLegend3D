#pragma once

#include "Engine_Define.h"

class CBase;

BEGIN(Engine)

class IRenderable
{
public:
	virtual void Render(LPDIRECT3DDEVICE9& pGraphicDev) PURE;
	virtual _float Get_ViewZ() PURE;
	virtual _float Get_Z() PURE; // 직교 투영용
	virtual CBase* GetBase() PURE;
};

END