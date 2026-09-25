#include "pch.h"
#include "CLaserBuffer.h"

CLaserBuffer::CLaserBuffer()
{
}

CLaserBuffer::CLaserBuffer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CLaserBuffer::CLaserBuffer(const CLaserBuffer& rhs)
	: CVIBuffer(rhs)
{
}

CLaserBuffer::~CLaserBuffer()
{
}

HRESULT CLaserBuffer::Ready_Buffer()
{
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = 4;
	m_dwTriCnt = 2;
	m_dwFVF = FVF_TEX;

	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXTEX* pVertex = NULL;

	/// &pVertex : 버텍스 버퍼에 저장된 버텍스 중 첫 번째 버텍스
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	/// 오른쪽 위

	pVertex[0].vPosition = { -0.5f, 0.f, 0.f };
	pVertex[0].vTexUV = { 0.f, 0.f };

	pVertex[1].vPosition = { 0.5f, 0.f, 0.f };
	pVertex[1].vTexUV = { 1.f, 0.f };

	pVertex[2].vPosition = { 0.5f, 0.f, -1.f };
	pVertex[2].vTexUV = { 1.f, 1.f };

	pVertex[3].vPosition = { -0.5f, 0.f, -1.f };
	pVertex[3].vTexUV = { 0.f, 1.f };

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

void CLaserBuffer::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CLaserBuffer* CLaserBuffer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLaserBuffer* pLaserBuffer = new CLaserBuffer(pGraphicDev);

	if (FAILED(pLaserBuffer->Ready_Buffer()))
	{
		Safe_Release(pLaserBuffer);
		MSG_BOX("LaserBuffer Create Failed");
		return nullptr;
	}

	return pLaserBuffer;
}

CComponent* CLaserBuffer::Clone()
{
	return new CLaserBuffer(*this);
}

void CLaserBuffer::Free()
{
	CVIBuffer::Free();
}
