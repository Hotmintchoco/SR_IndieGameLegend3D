#include "pch.h"
#include "CRibbonBuffer.h"

CRibbonBuffer::CRibbonBuffer()
{
}

CRibbonBuffer::CRibbonBuffer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CRibbonBuffer::~CRibbonBuffer()
{
}

HRESULT CRibbonBuffer::Ready_Buffer()
{
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = 2 * m_dwMaxSection;
	m_dwTriCnt = 2 * (m_dwMaxSection - 1);
	m_dwFVF = FVF_TEX;

	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_Buffer(D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY , D3DPOOL_DEFAULT)))
		return E_FAIL;

	INDEX16* pIndex = NULL;

	/* Index Buffer의 경우 determinant하므로 최대치까지 바로 그림 */
	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	for (DWORD i = 0; i < m_dwMaxSection - 1; ++i)
	{
		WORD L0 = 2 * i, R0 = L0 + 1, L1 = L0 + 2, R1 = L0 + 3;
		pIndex[i * 2 + 0] = { L0, L1, R1 };
		pIndex[i * 2 + 1] = { L0, R1, R0 };
	}

	m_pIB->Unlock();

	return S_OK;
}

void CRibbonBuffer::Render_Buffer()
{
	if (m_dwCurSection < 2)
		return;

	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);
	m_pGraphicDev->SetFVF(m_dwFVF);
	m_pGraphicDev->SetIndices(m_pIB);
	m_pGraphicDev->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, 0, 0,
		m_dwCurSection * 2,
		0,
		m_dwCurSection * 2);
}

void CRibbonBuffer::AddSegment(const _vec3& vLeft, const _vec3& vRight, const float fTraveled)
{
	if (m_dwCurSection >= m_dwMaxSection) return;

	VTXTEX* pVertex = NULL;

	/* offset : #Segment * Sizeof(VTXTEX) * 2Pts/Segment */
	m_pVB->Lock(m_dwCurSection * m_dwVtxSize * 2, m_dwVtxSize * 2, (void**)&pVertex, D3DLOCK_NOOVERWRITE);

	pVertex[0].vPosition = vLeft;
	pVertex[0].vTexUV = { 0.f, fTraveled };

	pVertex[1].vPosition = vRight;
	pVertex[1].vTexUV = { 1.f, fTraveled };

	m_pVB->Unlock();

	++m_dwCurSection;
}

void CRibbonBuffer::ReplaceHead(const _vec3& vLeft, const _vec3& vRight, const float fTraveled)
{
	if (m_dwCurSection >= m_dwMaxSection) return;

	VTXTEX* pVertex = NULL;

	/* offset : #Segment * Sizeof(VTXTEX) * 2Pts/Segment */
	m_pVB->Lock(m_dwCurSection * m_dwVtxSize * 2, m_dwVtxSize * 2, (void**)&pVertex, 0);

	pVertex[0].vPosition = vLeft;
	pVertex[0].vTexUV = { 0.f, fTraveled };

	pVertex[1].vPosition = vRight;
	pVertex[1].vTexUV = { 1.f, fTraveled };

	m_pVB->Unlock();
}

CRibbonBuffer* CRibbonBuffer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRibbonBuffer* pBuffer = new CRibbonBuffer(pGraphicDev);

	if (FAILED(pBuffer->Ready_Buffer()))
	{
		Safe_Release(pBuffer);
		MSG_BOX("CRibbonBuffer Create Failed");
		return nullptr;
	}

	return pBuffer;
}

void CRibbonBuffer::Free()
{
	CVIBuffer::Free();
}
