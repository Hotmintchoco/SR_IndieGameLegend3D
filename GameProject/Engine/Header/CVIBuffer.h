#pragma once
#include "CComponent.h"

BEGIN(Engine)

struct TVIBufferInfo
{
	D3DFORMAT idxFmt = D3DFMT_UNKNOWN;
	_ulong dwVtxSize = 0;
	_ulong dwIdxSize = 0;
	_ulong dwVtxCnt = 0;
	_ulong dwTriCnt = 0;
};

struct TTriInfo
{
	_vec3 vTriPos[3] = { {0.f, 0.f, 0.f} }; 
};

class ENGINE_DLL CVIBuffer : public CComponent
{
protected:
	explicit CVIBuffer();
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer();

public:
	virtual HRESULT		Ready_Buffer(DWORD dwVBUsage = 0, D3DPOOL eVBPool = D3DPOOL_MANAGED);
	virtual void		Render_Buffer();

	inline TVIBufferInfo GetInfo() const {
		return TVIBufferInfo {
			m_IdxFmt,
			m_dwVtxSize,
			m_dwIdxSize,
			m_dwVtxCnt,
			m_dwTriCnt,
		};
	}
	inline const vector<TTriInfo>& GetTri() const 
	{
		static const vector<TTriInfo> s_pTri;
		return m_pTri ? *m_pTri : s_pTri;
	}

protected:
	LPDIRECT3DVERTEXBUFFER9			m_pVB;

	_ulong		m_dwVtxSize;		// 정점의 크기
	_ulong		m_dwVtxCnt;			// 정점의 개수
	_ulong		m_dwTriCnt;			// 삼각형의 개수
	_ulong		m_dwFVF;			// 정점의 옵션

	LPDIRECT3DINDEXBUFFER9			m_pIB;
	_ulong			m_dwIdxSize;
	D3DFORMAT		m_IdxFmt;

	shared_ptr<const vector<TTriInfo>> m_pTri;

public:
	virtual void	Free();

};

END