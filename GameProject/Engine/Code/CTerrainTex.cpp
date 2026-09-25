#include "CTerrainTex.h"

CTerrainTex::CTerrainTex() : m_pPos(nullptr)
{
}

CTerrainTex::CTerrainTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev), m_pPos(nullptr)
{
}

CTerrainTex::CTerrainTex(const CTerrainTex& rhs)
	: CVIBuffer(rhs), m_hFile(rhs.m_hFile)
	, m_fH(rhs.m_fH), m_iH(rhs.m_iH)
	, m_pPos(rhs.m_pPos)
{
}

CTerrainTex::~CTerrainTex()
{
}

HRESULT		CTerrainTex::Ready_Buffer(const _ulong& dwVtxCntX,
										const _ulong& dwVtxCntZ,
										const _ulong& dwVtxItv)
{

	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = dwVtxCntX * dwVtxCntZ;
	m_dwTriCnt = (dwVtxCntX - 1) * (dwVtxCntZ - 1) * 2;
	m_dwFVF = FVF_TEX;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	m_pPos = new _vec3[m_dwVtxCnt];

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXTEX* pVertex = NULL;
	
	_ulong dwIndex = 0;

	/// &pVertex : 버텍스 버퍼에 저장된 버텍스 중 첫 번째 버텍스
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	

	for (_ulong i = 0; i < dwVtxCntZ; ++i)
	{
		for (_ulong j = 0; j < dwVtxCntX; ++j)
		{
			dwIndex = i * dwVtxCntX + j;

			pVertex[dwIndex].vPosition = { _float(j * dwVtxItv), 
										   0.f,
										   _float(i * dwVtxItv) };

			pVertex[dwIndex].vNormal = { 0.f, 0.f, 0.f };

			m_pPos[dwIndex] = pVertex[dwIndex].vPosition;

			pVertex[dwIndex].vTexUV = { _float(j) / (dwVtxCntX - 1) * 20.f, 
										_float(i) / (dwVtxCntZ - 1) * 20.f};

		}
	}

	_vec3	vNormal, vDst, vSrc;

	INDEX32* pIndex = NULL;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	_ulong dwTriCnt(0);

	for (_ulong i = 0; i < dwVtxCntZ - 1; ++i)
	{
		for (_ulong j = 0; j < dwVtxCntX - 1; ++j)
		{
			dwIndex = i * dwVtxCntX + j;
			
			// 오른쪽 위

			pIndex[dwTriCnt]._0 = dwIndex + dwVtxCntX;
			pIndex[dwTriCnt]._1 = dwIndex + dwVtxCntX + 1;
			pIndex[dwTriCnt]._2 = dwIndex + 1;

			vDst = pVertex[pIndex[dwTriCnt]._1].vPosition - pVertex[pIndex[dwTriCnt]._0].vPosition;
			vSrc = pVertex[pIndex[dwTriCnt]._2].vPosition - pVertex[pIndex[dwTriCnt]._1].vPosition;

			D3DXVec3Cross(&vNormal, &vDst, &vSrc);

			pVertex[pIndex[dwTriCnt]._0].vNormal += vNormal;
			pVertex[pIndex[dwTriCnt]._1].vNormal += vNormal;
			pVertex[pIndex[dwTriCnt]._2].vNormal += vNormal;
			
			dwTriCnt++;

			// 왼쪽 아래
			pIndex[dwTriCnt]._0 = dwIndex + dwVtxCntX;
			pIndex[dwTriCnt]._1 = dwIndex + 1;
			pIndex[dwTriCnt]._2 = dwIndex;

			vDst = pVertex[pIndex[dwTriCnt]._1].vPosition - pVertex[pIndex[dwTriCnt]._0].vPosition;
			vSrc = pVertex[pIndex[dwTriCnt]._2].vPosition - pVertex[pIndex[dwTriCnt]._1].vPosition;

			D3DXVec3Cross(&vNormal, &vDst, &vSrc);

			pVertex[pIndex[dwTriCnt]._0].vNormal += vNormal;
			pVertex[pIndex[dwTriCnt]._1].vNormal += vNormal;
			pVertex[pIndex[dwTriCnt]._2].vNormal += vNormal;

			dwTriCnt++;
		}
	}

	for (_uint i = 0; i < m_dwVtxCnt; ++i)
		D3DXVec3Normalize(&pVertex[i].vNormal, &pVertex[i].vNormal);


	m_pVB->Unlock();
	m_pIB->Unlock();

	return S_OK;
}

void CTerrainTex::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CTerrainTex* CTerrainTex::Create(LPDIRECT3DDEVICE9 pGraphicDev,
								const _ulong& dwVtxCntX,
								const _ulong& dwVtxCntZ,
								const _ulong& dwVtxItv)
{
	CTerrainTex* pTerrainTex = new CTerrainTex(pGraphicDev);

	if (FAILED(pTerrainTex->Ready_Buffer(dwVtxCntX, dwVtxCntZ, dwVtxItv)))
	{
		Safe_Release(pTerrainTex);
		MSG_BOX("TerrainTex Create Failed");
		return nullptr;
	}

	return pTerrainTex;
}

CComponent* CTerrainTex::Clone()
{
	return new CTerrainTex(*this);
}

void CTerrainTex::Free()
{
	if (false == m_bClone)
	{
		Safe_Delete_Array(m_pPos);
	}

	CVIBuffer::Free();
}
