#include "pch.h"
#include "CFlatPlayerTex.h"

CFlatPlayerTex::CFlatPlayerTex()
{
}

CFlatPlayerTex::CFlatPlayerTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CFlatPlayerTex::CFlatPlayerTex(const CFlatPlayerTex& rhs)
	: CVIBuffer(rhs)
{
}

CFlatPlayerTex::~CFlatPlayerTex()
{
}

HRESULT CFlatPlayerTex::Ready_Buffer(DWORD dwVBUsage, D3DPOOL eVBPool)
{
	m_dwFVF = FVF_TEX;
	m_dwVtxCnt = 4;
	m_dwTriCnt = 2;
	m_dwVtxSize = sizeof(VTXTEX);

	m_IdxFmt = D3DFMT_INDEX32;
	m_dwIdxSize = sizeof(INDEX32);

	if (FAILED(CVIBuffer::Ready_Buffer(dwVBUsage, eVBPool)))
		return E_FAIL;

	// XZ 평면, y = 0, 한 변 길이 1
	// 원점 중심 quad를 z 방향으로 +0.5 이동 → x : [-0.5, 0.5], z : [0, 1]
	//
	//   z
	//   ^   0 ---- 1      (z = 1)
	//   |   |    / |
	//   |   |  /   |
	//   |   3 ---- 2      (z = 0)
	//   +---------> x
	//     -0.5   0.5
	//
	// 위(+Y)에서 내려다봤을 때 시계방향 → 기본 컬링(D3DCULL_CCW)에서 앞면
	const _vec3 vPos[4] =
	{
		{ -0.5f, 0.f, 1.f },
		{  0.5f, 0.f, 1.f },
		{  0.5f, 0.f, 0.f },
		{ -0.5f, 0.f, 0.f },
	};

	VTXTEX* pVertex = nullptr;
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = vPos[0];
	pVertex[0].vTexUV = { 0.f, 0.f };

	pVertex[1].vPosition = vPos[1];
	pVertex[1].vTexUV = { 1.f, 0.f };

	pVertex[2].vPosition = vPos[2];
	pVertex[2].vTexUV = { 1.f, 1.f };

	pVertex[3].vPosition = vPos[3];
	pVertex[3].vTexUV = { 0.f, 1.f };

	m_pVB->Unlock();

	INDEX32* pIndex = nullptr;
	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// 오른쪽 위 삼각형
	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	// 왼쪽 아래 삼각형
	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();

	// 피킹/충돌용 삼각형 정보 (클론끼리 공유)
	vector<TTriInfo> vecTri(m_dwTriCnt);

	vecTri[0].vTriPos[0] = vPos[0];
	vecTri[0].vTriPos[1] = vPos[1];
	vecTri[0].vTriPos[2] = vPos[2];

	vecTri[1].vTriPos[0] = vPos[0];
	vecTri[1].vTriPos[1] = vPos[2];
	vecTri[1].vTriPos[2] = vPos[3];

	m_pTri = make_shared<const vector<TTriInfo>>(std::move(vecTri));

	return S_OK;
}

void CFlatPlayerTex::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CFlatPlayerTex* CFlatPlayerTex::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFlatPlayerTex* pInstance = new CFlatPlayerTex(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CFlatPlayerTex Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CFlatPlayerTex::Clone()
{
	return new CFlatPlayerTex(*this);
}

void CFlatPlayerTex::Free()
{
	CVIBuffer::Free();
}