#pragma once

#include "CVIBuffer.h"

class COpenCubeTex : public Engine::CVIBuffer
{
private:
	explicit COpenCubeTex();
	explicit COpenCubeTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit COpenCubeTex(const COpenCubeTex& rhs);
	virtual ~COpenCubeTex();

public:
	virtual HRESULT Ready_Buffer(DWORD dwVBUsage = 0, D3DPOOL eVBPool = D3DPOOL_MANAGED) override;
	virtual void    Render_Buffer() override;

public:
	static COpenCubeTex* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone() override;

private:
	virtual void Free() override;
};