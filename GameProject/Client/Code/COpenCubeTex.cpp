#include "pch.h"
#include "COpenCubeTex.h"

COpenCubeTex::COpenCubeTex()
{
}

COpenCubeTex::COpenCubeTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

COpenCubeTex::COpenCubeTex(const COpenCubeTex& rhs)
	: CVIBuffer(rhs)
{
}

COpenCubeTex::~COpenCubeTex()
{
}

HRESULT COpenCubeTex::Ready_Buffer(DWORD dwVBUsage, D3DPOOL eVBPool)
{
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = 20;	// 5면 x 4
	m_dwTriCnt = 10;	// 5면 x 2
	m_dwFVF = FVF_TEX;

	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	/// 각 면 : 바깥에서 봤을 때 좌상(0,0) → 우상(1,0) → 우하(1,1) → 좌하(0,1)
	struct TFace
	{
		_vec3 v[4];
		_vec3 vNormal;
	};

	const TFace tFaces[5] =
	{
		// X+
		{ { {  0.5f,  0.5f, -0.5f }, {  0.5f,  0.5f,  0.5f }, {  0.5f, -0.5f,  0.5f }, {  0.5f, -0.5f, -0.5f } }, {  1.f,  0.f,  0.f } },
		// X-
		{ { { -0.5f,  0.5f,  0.5f }, { -0.5f,  0.5f, -0.5f }, { -0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f,  0.5f } }, { -1.f,  0.f,  0.f } },
		// Y- (윗면 Y+ 는 없음)
		{ { { -0.5f, -0.5f, -0.5f }, {  0.5f, -0.5f, -0.5f }, {  0.5f, -0.5f,  0.5f }, { -0.5f, -0.5f,  0.5f } }, {  0.f, -1.f,  0.f } },
		// Z+
		{ { {  0.5f,  0.5f,  0.5f }, { -0.5f,  0.5f,  0.5f }, { -0.5f, -0.5f,  0.5f }, {  0.5f, -0.5f,  0.5f } }, {  0.f,  0.f,  1.f } },
		// Z-
		{ { { -0.5f,  0.5f, -0.5f }, {  0.5f,  0.5f, -0.5f }, {  0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f, -0.5f } }, {  0.f,  0.f, -1.f } },
	};

	const _vec2 vUV[4] = { { 0.f, 0.f }, { 1.f, 0.f }, { 1.f, 1.f }, { 0.f, 1.f } };

	VTXTEX* pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	for (_ulong f = 0; f < 5; ++f)
	{
		for (_ulong i = 0; i < 4; ++i)
		{
			pVertex[f * 4 + i].vPosition = tFaces[f].v[i];
			pVertex[f * 4 + i].vNormal = tFaces[f].vNormal;
			pVertex[f * 4 + i].vTexUV = vUV[i];
		}
	}

	m_pVB->Unlock();

	INDEX16* pIndex = NULL;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	for (_ushort f = 0; f < 5; ++f)
	{
		const _ushort b = f * 4;

		// 오른쪽 위
		pIndex[f * 2 + 0]._0 = b + 0;
		pIndex[f * 2 + 0]._1 = b + 1;
		pIndex[f * 2 + 0]._2 = b + 2;

		// 왼쪽 아래
		pIndex[f * 2 + 1]._0 = b + 0;
		pIndex[f * 2 + 1]._1 = b + 2;
		pIndex[f * 2 + 1]._2 = b + 3;
	}

	m_pIB->Unlock();

	return S_OK;
}

void COpenCubeTex::Render_Buffer()
{
	// 위가 뚫려 있으니 안쪽 벽(뒷면)도 보여야 함 → 컬링 해제
	DWORD dwPrevCull = 0;
	m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &dwPrevCull);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	CVIBuffer::Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, dwPrevCull);
}

COpenCubeTex* COpenCubeTex::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	COpenCubeTex* pOpenCubeTex = new COpenCubeTex(pGraphicDev);

	if (FAILED(pOpenCubeTex->Ready_Buffer()))
	{
		Safe_Release(pOpenCubeTex);
		MSG_BOX("OpenCubeTex Create Failed");
		return nullptr;
	}

	return pOpenCubeTex;
}

CComponent* COpenCubeTex::Clone()
{
	return new COpenCubeTex(*this);
}

void COpenCubeTex::Free()
{
	CVIBuffer::Free();
}