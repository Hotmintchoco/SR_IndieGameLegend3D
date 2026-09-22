#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CCustomTex : public CVIBuffer
{
protected:
	explicit CCustomTex();
	explicit CCustomTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCustomTex(const CCustomTex& rhs);
	virtual ~CCustomTex();

public:
	virtual HRESULT		Ready_Buffer(vector<VTXTEX>& vecVtx, vector<INDEX16>& vecIdx);
	virtual HRESULT		Ready_Buffer(vector<VTXTEX_NORMAL_NONE>& vecVtx, vector<INDEX16>& vecIdx);
	virtual void		Render_Buffer();

public:
	static CCustomTex* Create(LPDIRECT3DDEVICE9 pGraphicDev, vector<VTXTEX>& vecVtx, vector<INDEX16>& vecIdx);
	static CCustomTex* Create(LPDIRECT3DDEVICE9 pGraphicDev, vector<VTXTEX_NORMAL_NONE>& vecVtx, vector<INDEX16>& vecIdx);

	virtual CComponent* Clone();

private:
	virtual void Free();
};

END