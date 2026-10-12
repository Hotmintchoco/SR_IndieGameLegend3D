#pragma once
#include "CMG0_DancerPart.h"

class CMG0_DancerBody : public CMG0_DancerPart
{
protected:
	explicit CMG0_DancerBody(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMG0_DancerBody();

public:
	virtual			HRESULT		Ready_GameObject();

public:
	static CMG0_DancerBody* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};
