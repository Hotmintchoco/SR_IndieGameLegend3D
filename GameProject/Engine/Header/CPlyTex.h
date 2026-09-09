#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CPlyTex : public CVIBuffer
{
protected:
	explicit CPlyTex();
	explicit CPlyTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlyTex(const CPlyTex& rhs);
	virtual ~CPlyTex();

public:
	virtual HRESULT		Ready_Buffer();
	/* 파일 경로가 필요하다 보니 불가피하게 함수를 분리 */
	HRESULT	Ready_Buffer(const _tchar* szFilePath);
	virtual void		Render_Buffer();

public:
	static CPlyTex* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* szFilePath);
	virtual CComponent* Clone();

private:
	HRESULT Parse_Ply(const _tchar* szFilePath);

	vector<VTXTEX> m_vecVtx;
	vector<unsigned long> m_vecIdx;

private:
	virtual void Free();
};

END