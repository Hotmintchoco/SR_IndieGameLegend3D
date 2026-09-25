#include "CRcColCustom.h"

CRcColCustom::CRcColCustom()
{
}

CRcColCustom::CRcColCustom(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

//CRcColCustom::CRcColCustom(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3(&vec3)[4], const D3DXCOLOR(&color)[4])
//	: CVIBuffer(pGraphicDev)
//{
//	memcpy(m_vec, vec3, sizeof(m_vec));
//	memcpy(m_color, color, sizeof(m_color));
//}

CRcColCustom::CRcColCustom(const CRcColCustom& rhs)
	: CVIBuffer(rhs)
{
}

CRcColCustom::~CRcColCustom()
{
}

HRESULT CRcColCustom::Ready_Buffer()
{
	m_dwVtxSize = sizeof(VTXCOL);
	m_dwVtxCnt = 4;
	m_dwTriCnt = 2;
	m_dwFVF = FVF_COL;

	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXCOL* pVertex = NULL;

	/// &pVertex : 버텍스 버퍼에 저장된 버텍스 중 첫 번째 버텍스
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	/// 오른쪽 위

	pVertex[0].vPosition = { -1.f, 1.f, 0.f };
	pVertex[0].dwColor = D3DXCOLOR(1.f, 1.f, 0.f, 1.f);

	pVertex[1].vPosition = { 1.f, 1.f, 0.f };
	pVertex[1].dwColor = D3DXCOLOR(1.f, 1.f, 0.f, 1.f);

	pVertex[2].vPosition = { 1.f, -1.f, 0.f };
	pVertex[2].dwColor = D3DXCOLOR(1.f, 1.f, 0.f, 1.f);

	pVertex[3].vPosition = { -1.f, -1.f, 0.f };
	pVertex[3].dwColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);

	//for (int i = 0; i < 4; ++i)
	//{
	//	pVertex[i].vPosition = m_vec[i];
	//	pVertex[i].dwColor = m_color[i];
	//}

	m_pVB->Unlock();

	INDEX16* pIndex = NULL;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// 오른쪽 위
	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	// 왼쪽 아래
	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();


	return S_OK;
}

void CRcColCustom::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

HRESULT CRcColCustom::Set_Buffer(const _vec3(&vec3)[4], const D3DXCOLOR(&color)[4])
{
	m_dwVtxSize = sizeof(VTXCOL);
	m_dwVtxCnt = 4;
	m_dwTriCnt = 2;
	m_dwFVF = FVF_COL;

	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXCOL* pVertex = NULL;

	/// &pVertex : 버텍스 버퍼에 저장된 버텍스 중 첫 번째 버텍스
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	/// 오른쪽 위

	for (int i = 0; i < 4; ++i)
	{
		pVertex[i].vPosition = vec3[i];
		pVertex[i].dwColor = color[i];
	}

	m_pVB->Unlock();

	INDEX16* pIndex = NULL;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// 오른쪽 위
	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	// 왼쪽 아래
	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();


	return S_OK;
}

CRcColCustom* CRcColCustom::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRcColCustom* pRcCol = new CRcColCustom(pGraphicDev);

	if (FAILED(pRcCol->Ready_Buffer()))
	{
		Safe_Release(pRcCol);
		MSG_BOX("RcColCustom Create Failed");
		return nullptr;
	}

	return pRcCol;
}

CComponent* CRcColCustom::Clone()
{
	return new CRcColCustom(*this);
}

void CRcColCustom::Free()
{
	CVIBuffer::Free();
}
