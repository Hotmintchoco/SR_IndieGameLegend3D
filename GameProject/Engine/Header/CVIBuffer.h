#pragma once
#include "CComponent.h"

BEGIN(Engine)

struct TVIBufferInfo
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuffer = nullptr;
	LPDIRECT3DINDEXBUFFER9 pIdxBuffer = nullptr;
	VTXSTRUCTTYPE vtxStructType = VTXSTRUCT_DEFAULT;
	D3DFORMAT idxFmt = D3DFMT_UNKNOWN;
	_ulong dwVtxSize = 0;
	_ulong dwIdxSize = 0;
	_ulong dwVtxCnt = 0;
	_ulong dwTriCnt = 0;
	
};

class ENGINE_DLL CVIBuffer : public CComponent
{
protected:
	explicit CVIBuffer();
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer();

public:
	virtual HRESULT		Ready_Buffer();
	virtual void		Render_Buffer();

protected:
	LPDIRECT3DVERTEXBUFFER9			m_pVB;

	_ulong		m_dwVtxSize;		// 정점의 크기
	_ulong		m_dwVtxCnt;			// 정점의 개수
	_ulong		m_dwTriCnt;			// 삼각형의 개수
	_ulong		m_dwFVF;			// 정점의 옵션

	LPDIRECT3DINDEXBUFFER9			m_pIB;
	VTXSTRUCTTYPE	m_VtxStructType;
	_ulong			m_dwIdxSize;
	D3DFORMAT		m_IdxFmt;

public : 
	//void	Get_VIInfo	(LPDIRECT3DVERTEXBUFFER9& pVtxBuffer,
	//					LPDIRECT3DINDEXBUFFER9& pIdxBuffer,
	//					VTXSTRUCTTYPE& vtxStructType,
	//					D3DFORMAT& idxFmt,
	//					_ulong& dwVtxCnt,
	//					_ulong& dwTriCnt)
	//{
	//	pVtxBuffer = m_pVB;
	//	pIdxBuffer = m_pIB;
	//	vtxStructType = m_VtxStructType;
	//	idxFmt = m_IdxFmt;
	//	dwVtxCnt = m_dwVtxCnt;
	//	dwTriCnt = m_dwTriCnt;
	//}

	void	Get_VIInfo	(TVIBufferInfo* t)
	{
		t->pVtxBuffer = m_pVB;
		t->pIdxBuffer = m_pIB;
		t->vtxStructType = m_VtxStructType;
		t->idxFmt = m_IdxFmt;
		t->dwVtxSize = m_dwVtxSize;
		t->dwIdxSize = m_dwIdxSize;
		t->dwVtxCnt = m_dwVtxCnt;
		t->dwTriCnt = m_dwTriCnt;
	}


public:
	virtual void	Free();

};

END