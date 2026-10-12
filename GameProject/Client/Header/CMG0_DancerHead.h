#pragma once
#include "CMG0_DancerPart.h"

class CMG0_DancerHead : public CMG0_DancerPart
{
protected:
	explicit CMG0_DancerHead(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMG0_DancerHead();

public:
	virtual			HRESULT		Ready_GameObject();

public:
	static CMG0_DancerHead* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};
