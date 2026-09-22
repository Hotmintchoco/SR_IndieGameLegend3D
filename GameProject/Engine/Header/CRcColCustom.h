#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CRcColCustom : public CVIBuffer
{
protected:
	explicit CRcColCustom();
	explicit CRcColCustom(LPDIRECT3DDEVICE9 pGraphicDev);
	//explicit CRcColCustom(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3 (&vec3)[4], const D3DXCOLOR (&color)[4]);
	explicit CRcColCustom(const CRcColCustom& rhs);
	virtual ~CRcColCustom();

public:
	virtual HRESULT		Ready_Buffer();
	virtual void		Render_Buffer();

	HRESULT Set_Buffer(const _vec3(&vec3)[4], const D3DXCOLOR(&color)[4]);

public:
	static CRcColCustom* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual void Free();
	//_vec3 m_vec[4];
	//D3DXCOLOR m_color[4];
};

END