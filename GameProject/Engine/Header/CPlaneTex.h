#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CPlaneTex : public CVIBuffer
{
protected:
	explicit CPlaneTex();
	explicit CPlaneTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlaneTex(const CPlaneTex& rhs);
	virtual ~CPlaneTex();

public:
	virtual HRESULT		Ready_Buffer();
	virtual void		Render_Buffer();

public:
	static CPlaneTex* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual void Free();
};

END