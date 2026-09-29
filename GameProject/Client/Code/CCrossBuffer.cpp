#include "pch.h"
#include "CCrossBuffer.h"

CCrossBuffer::CCrossBuffer()
{
}

CCrossBuffer::CCrossBuffer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CCrossBuffer::CCrossBuffer(const CCrossBuffer& rhs)
	: CVIBuffer(rhs)
{
}

CCrossBuffer::~CCrossBuffer()
{
}

HRESULT CCrossBuffer::Ready_Buffer()
{
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = 8;
	m_dwTriCnt = 4;
	m_dwFVF = FVF_TEX;

	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXTEX* pVertex = NULL;

	/// &pVertex : 버텍스 버퍼에 저장된 버텍스 중 첫 번째 버텍스
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	/// 오른쪽 위

	pVertex[0].vPosition = { -0.5f, 0.f, 0.5f };
	pVertex[0].vTexUV = { 0.f, 0.f };

	pVertex[1].vPosition = { 0.5f, 0.f, 0.5f };
	pVertex[1].vTexUV = { 1.f, 0.f };

	pVertex[2].vPosition = { 0.5f, 0.f, -0.5f };
	pVertex[2].vTexUV = { 1.f, 1.f };

	pVertex[3].vPosition = { -0.5f, 0.f, -0.5f };
	pVertex[3].vTexUV = { 0.f, 1.f };

	pVertex[4].vPosition = { 0.f, 0.5f, 0.5f };
	pVertex[4].vTexUV = { 0.f, 0.f };

	pVertex[5].vPosition = { 0.f, -0.5f, 0.5f };
	pVertex[5].vTexUV = { 1.f, 0.f };

	pVertex[6].vPosition = { 0.f, -0.5f, -0.5f };
	pVertex[6].vTexUV = { 1.f, 1.f };

	pVertex[7].vPosition = { 0.f, 0.5f, -0.5f };
	pVertex[7].vTexUV = { 0.f, 1.f };

	m_pVB->Unlock();

	INDEX16* pIndex = NULL;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	pIndex[2]._0 = 4;
	pIndex[2]._1 = 5;
	pIndex[2]._2 = 6;

	pIndex[3]._0 = 4;
	pIndex[3]._1 = 6;
	pIndex[3]._2 = 7;

	m_pIB->Unlock();


	return S_OK;
}

void CCrossBuffer::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CCrossBuffer* CCrossBuffer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCrossBuffer* pBuffer = new CCrossBuffer(pGraphicDev);

	if (FAILED(pBuffer->Ready_Buffer()))
	{
		Safe_Release(pBuffer);
		MSG_BOX("CCrossBuffer Create Failed");
		return nullptr;
	}

	return pBuffer;
}

CComponent* CCrossBuffer::Clone()
{
	return new CCrossBuffer(*this);
}

void CCrossBuffer::Free()
{
	CVIBuffer::Free();
}
