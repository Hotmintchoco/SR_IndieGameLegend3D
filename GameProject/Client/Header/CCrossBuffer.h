#pragma once
#include "CVIBuffer.h"

class CCrossBuffer : public CVIBuffer
{
protected:
	explicit CCrossBuffer();
	explicit CCrossBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCrossBuffer(const CCrossBuffer& rhs);
	virtual ~CCrossBuffer();

public:
	virtual HRESULT		Ready_Buffer();
	virtual void		Render_Buffer();

public:
	static CCrossBuffer* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual void Free() override;
};