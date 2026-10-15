#pragma once

#include "CVIBuffer.h"

class CFlatPlayerTex : public CVIBuffer
{
private:
	explicit CFlatPlayerTex();
	explicit CFlatPlayerTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CFlatPlayerTex(const CFlatPlayerTex& rhs);
	virtual ~CFlatPlayerTex();

public:
	virtual HRESULT		Ready_Buffer(DWORD dwVBUsage = 0, D3DPOOL eVBPool = D3DPOOL_MANAGED) override;
	virtual void		Render_Buffer() override;

public:
	static CFlatPlayerTex* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual void	Free() override;
};