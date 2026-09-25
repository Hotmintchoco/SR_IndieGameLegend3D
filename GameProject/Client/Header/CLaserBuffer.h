#pragma once
#include "CVIBuffer.h"

class CLaserBuffer : public CVIBuffer
{
protected:
	explicit CLaserBuffer();
	explicit CLaserBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CLaserBuffer(const CLaserBuffer& rhs);
	virtual ~CLaserBuffer();

public:
	virtual HRESULT		Ready_Buffer();
	virtual void		Render_Buffer();

public:
	static CLaserBuffer* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual void Free() override;
};