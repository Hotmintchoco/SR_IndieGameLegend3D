#include "pch.h"
#include "CPlayerPartTex.h"


CPlayerPartTex::CPlayerPartTex()
{
}

CPlayerPartTex::CPlayerPartTex(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPivot)
	: CPlyTex(pGraphicDev), m_vPivot(vPivot)
{
}

CPlayerPartTex::CPlayerPartTex(const CPlayerPartTex& rhs)
	: CPlyTex(rhs), m_vPivot(rhs.m_vPivot)
{
}

CPlayerPartTex::~CPlayerPartTex()
{
}

HRESULT CPlayerPartTex::Ready_Buffer(const _tchar* szFilePath)
{
	if (FAILED(Parse_Ply(szFilePath)))
	{
		MSG_BOX("[CPlyTex] Ply Parse Failed");
	}

	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = static_cast<DWORD>(m_vecVtx.size());
	m_dwTriCnt = static_cast<DWORD>(m_vecIdx.size() / 3);
	m_dwFVF = FVF_TEX;
	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;

	auto pTri = std::make_shared<std::vector<TTriInfo>>();
	pTri->reserve(m_dwTriCnt);

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXTEX* pVertex = nullptr;
	if (FAILED(m_pVB->Lock(0, 0, (void**)&pVertex, 0)))
		return E_FAIL;

	for (size_t i = 0; i < m_vecVtx.size(); ++i)
	{
		pVertex[i].vPosition = m_vecVtx[i].vPosition - m_vPivot;
		pVertex[i].vNormal = m_vecVtx[i].vNormal;
		pVertex[i].vTexUV = m_vecVtx[i].vTexUV;
	}

	m_pVB->Unlock();

	INDEX16* pIndex = nullptr;
	if (FAILED(m_pIB->Lock(0, 0, (void**)&pIndex, 0)))
		return E_FAIL;

	for (DWORD i = 0; i < m_dwTriCnt; ++i)
	{
		pIndex[i]._0 = (_ushort)m_vecIdx[i * 3 + 0];
		pIndex[i]._1 = (_ushort)m_vecIdx[i * 3 + 1];
		pIndex[i]._2 = (_ushort)m_vecIdx[i * 3 + 2];

		pTri->push_back(TTriInfo{
			pVertex[pIndex[i]._0].vPosition,
			pVertex[pIndex[i]._1].vPosition,
			pVertex[pIndex[i]._2].vPosition,
			});
	}

	m_pTri = move(pTri);

	m_pIB->Unlock();

	return S_OK;

}

CPlayerPartTex* CPlayerPartTex::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* szFilePath, const _vec3& vPivot)
{
	CPlayerPartTex* pBuffer = new CPlayerPartTex(pGraphicDev, vPivot);

	if (FAILED(pBuffer->Ready_Buffer(szFilePath)))
	{
		_tchar szMsg[MAX_PATH + 64] = L"";
		swprintf_s(szMsg, L"CPlayerPartTex Create Failed\n%s", szFilePath);
		MessageBox(NULL, szMsg, L"System Message", MB_OK);

		Safe_Release(pBuffer);
		return nullptr;
	}

	return pBuffer;
}

CComponent* CPlayerPartTex::Clone()
{
	return new CPlayerPartTex(*this);
}

void CPlayerPartTex::Free()
{
	CPlyTex::Free();
}
