#include "CCustomTex.h"

CCustomTex::CCustomTex()
{
}

CCustomTex::CCustomTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CCustomTex::CCustomTex(const CCustomTex& rhs)
	: CVIBuffer(rhs)
{
}

CCustomTex::~CCustomTex()
{
}

HRESULT CCustomTex::Ready_Buffer(vector<VTXTEX>& vecVtx, vector<INDEX16>& vecIdx)
{
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = vecVtx.size();
	m_dwTriCnt = vecIdx.size();
	m_dwFVF = FVF_TEX;

	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;
	m_VtxStructType = VTXSTRUCT_TEX;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXTEX* pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	for (_int i = 0; i < m_dwVtxCnt; i++)
	{
		pVertex[i].vPosition = vecVtx[i].vPosition;
		pVertex[i].vNormal = vecVtx[i].vNormal;
		pVertex[i].vTexUV = vecVtx[i].vTexUV;
	}

	INDEX16* pIndex = NULL;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	for (_int j = 0; j < m_dwTriCnt; j++)
	{
		pIndex[j]._0 = vecIdx[j]._0;
		pIndex[j]._1 = vecIdx[j]._1;
		pIndex[j]._2 = vecIdx[j]._2;
	}

	m_pVB->Unlock();
	m_pIB->Unlock();

	return S_OK;
}

HRESULT CCustomTex::Ready_Buffer(vector<VTXTEX_NORMAL_NONE>& vecVtx, vector<INDEX16>& vecIdx)
{
	m_dwVtxSize = sizeof(VTXTEX_NORMAL_NONE);
	m_dwVtxCnt = vecVtx.size();
	m_dwTriCnt = vecIdx.size();
	m_dwFVF = FVF_TEX_NORMAL_NONE;

	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;
	m_VtxStructType = VTXSTRUCT_TEX_NORMAL_NONE;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXTEX_NORMAL_NONE* pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	for (_int i = 0; i < m_dwVtxCnt; i++)
	{
		pVertex[i].vPosition = vecVtx[i].vPosition;
		pVertex[i].vTexUV = vecVtx[i].vTexUV;
	}

	INDEX16* pIndex = NULL;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	for (_int j = 0; j < m_dwTriCnt; j++)
	{
		pIndex[j]._0 = vecIdx[j]._0;
		pIndex[j]._1 = vecIdx[j]._1;
		pIndex[j]._2 = vecIdx[j]._2;
	}

	m_pVB->Unlock();
	m_pIB->Unlock();

	return S_OK;
}

void CCustomTex::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CCustomTex* CCustomTex::Create(LPDIRECT3DDEVICE9 pGraphicDev, vector<VTXTEX>& vecVtx, vector<INDEX16>& vecIdx)
{
	CCustomTex* pPlaneTex = new CCustomTex(pGraphicDev);

	if (FAILED(pPlaneTex->Ready_Buffer(vecVtx, vecIdx)))
	{
		Safe_Release(pPlaneTex);
		MSG_BOX("CustomTex Create Failed");
		return nullptr;
	}

	return pPlaneTex;
}

CCustomTex* CCustomTex::Create(LPDIRECT3DDEVICE9 pGraphicDev, vector<VTXTEX_NORMAL_NONE>& vecVtx, vector<INDEX16>& vecIdx)
{
	CCustomTex* pPlaneTex = new CCustomTex(pGraphicDev);

	if (FAILED(pPlaneTex->Ready_Buffer(vecVtx, vecIdx)))
	{
		Safe_Release(pPlaneTex);
		MSG_BOX("CustomTex Create Failed");
		return nullptr;
	}

	return pPlaneTex;
}

CComponent* CCustomTex::Clone()
{
	return new CCustomTex(*this);
}

void CCustomTex::Free()
{
	CVIBuffer::Free();
}




// 플레이어 데미지 입을수있도록
// 플레이어 피격무시
// 플레이어 사망 및 부활
// 플레이어 몬스터 충돌 시 데미지
// 총알 2개 다enum
// 빨개지는거