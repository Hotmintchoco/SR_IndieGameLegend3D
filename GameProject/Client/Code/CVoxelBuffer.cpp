#include "pch.h"
#include "CVoxelBuffer.h"

CVoxelBuffer::CVoxelBuffer(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrTexturePath)
	: CVIBuffer(pGraphicDev), m_wstrTexturePath(wstrTexturePath)
{
}

CVoxelBuffer::CVoxelBuffer(const CVoxelBuffer& rhs)
	: CVIBuffer(rhs)
{
}

CVoxelBuffer::~CVoxelBuffer()
{
}

HRESULT CVoxelBuffer::Ready_Buffer()
{
	LPDIRECT3DTEXTURE9 pTexture = nullptr;
	D3DXIMAGE_INFO tInfo{}; /* 이미지 크기를 얻기 위함 */

	if (FAILED(D3DXCreateTextureFromFileEx(
		m_pGraphicDev, m_wstrTexturePath.c_str(),
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, // 원본 크기 그대로
		1, 0,                                       // 밉맵 1단계, Usage 없음
		D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH,           // 32비트 ARGB, CPU 전용
		D3DX_FILTER_NONE, D3DX_FILTER_NONE,         // 리샘플링 금지
		0, &tInfo, nullptr, &pTexture)))
		return E_FAIL;

	D3DLOCKED_RECT tRect{};
	pTexture->LockRect(0, &tRect, nullptr, D3DLOCK_READONLY);

	vector<BYTE> vecAlpha(tInfo.Width * tInfo.Height);

	BYTE* pBits = (BYTE*)tRect.pBits; /* 이미지 픽셀 바이트 시작 위치를 얻기 위함 */
	for (int y = 0; y < tInfo.Height; ++y)
	{
		for (int x = 0; x < tInfo.Width; ++x)
		{
			DWORD dwPix = *(DWORD*)(pBits + y * tRect.Pitch + x * 4); /* 패딩이 있는 경우를 대비하여 단순 Width 대신 Pitch 사용 */
			BYTE byAlpha = BYTE(dwPix >> 24);

			vecAlpha[y * tInfo.Width + x] = byAlpha;
		}
	}

	vector<TSideQuadInfo> vecQuad;
	float fOX = (float)tInfo.Width / 2.f;
	float fOY = (float)tInfo.Height / 2.f;

	/* 양쪽 메인 쿼드 */
	TSideQuadInfo t = {};
	t.vertex[0].vPosition = {-0.5f, fOY, fOX };
	t.vertex[0].vTexUV = { 0.f, 0.f };
	t.vertex[1].vPosition = { -0.5f, fOY, -fOX };
	t.vertex[1].vTexUV = { 1.f, 0.f };
	t.vertex[2].vPosition = { -0.5f, -fOY, -fOX };
	t.vertex[2].vTexUV = { 1.f, 1.f };
	t.vertex[3].vPosition = { -0.5f, -fOY, fOX };
	t.vertex[3].vTexUV = { 0.f, 1.f };
	t.index[0]._0 = 0;
	t.index[0]._1 = 1;
	t.index[0]._2 = 2;
	t.index[1]._0 = 0;
	t.index[1]._1 = 2;
	t.index[1]._2 = 3;
	vecQuad.push_back(t);

	t = {};
	t.vertex[0].vPosition = { 0.5f, fOY, fOX };
	t.vertex[0].vTexUV = { 0.f, 0.f };
	t.vertex[1].vPosition = { 0.5f, fOY, -fOX };
	t.vertex[1].vTexUV = { 1.f, 0.f };
	t.vertex[2].vPosition = { 0.5f, -fOY, -fOX };
	t.vertex[2].vTexUV = { 1.f, 1.f };
	t.vertex[3].vPosition = { 0.5f, -fOY, fOX };
	t.vertex[3].vTexUV = { 0.f, 1.f };
	t.index[0]._0 = 0;
	t.index[0]._1 = 2;
	t.index[0]._2 = 1;
	t.index[1]._0 = 0;
	t.index[1]._1 = 3;
	t.index[1]._2 = 2;
	vecQuad.push_back(t);

	vector<TDelta2D> tDelta = { {-1, 0}, {1, 0}, {0, 1}, {0, -1} };
	for (int y = 0; y < (int)tInfo.Height; ++y)
	{
		for (int x = 0; x < (int)tInfo.Width; ++x)
		{
			DWORD dwPix = *(DWORD*)(pBits + y * tRect.Pitch + x * 4); /* 패딩이 있는 경우를 대비하여 단순 Width 대신 Pitch 사용 */
			BYTE byAlpha = BYTE(dwPix >> 24);

			/* 공백이 아니면서 인접한 픽셀이 비어 있는 경우 */
			if (byAlpha == 0) continue;

			/* 4방향 비교 */
			for (auto& d : tDelta)
			{
				int iCmpX = x + d.dx;
				int iCmpY = y + d.dy;
				
				bool bInValidIdx = iCmpY < 0 || iCmpY >= tInfo.Height || iCmpX < 0 || iCmpX >= tInfo.Width;
				if (bool bTop = iCmpY < 0 || (!bInValidIdx && vecAlpha[iCmpY * tInfo.Width + iCmpX] == 0 && d.dy == -1))
				{
					t = GetSideQuadInfo(x, y, EQuadDirection::TOP, tInfo);
					vecQuad.push_back(t);
				}
				if (bool bBottom = iCmpY >= tInfo.Height || (!bInValidIdx && vecAlpha[iCmpY * tInfo.Width + iCmpX] == 0 && d.dy == 1))
				{
					t = GetSideQuadInfo(x, y, EQuadDirection::BOTTOM, tInfo);
					vecQuad.push_back(t);
				}
				if (bool bLeft = iCmpX < 0 || (!bInValidIdx && vecAlpha[iCmpY * tInfo.Width + iCmpX] == 0 && d.dx == -1))
				{
					t = GetSideQuadInfo(x, y, EQuadDirection::LEFT, tInfo);
					vecQuad.push_back(t);
				}
				if (bool bRight = iCmpX >= tInfo.Width || (!bInValidIdx && vecAlpha[iCmpY * tInfo.Width + iCmpX] == 0 && d.dx == 1))
				{
					t = GetSideQuadInfo(x, y, EQuadDirection::RIGHT, tInfo);
					vecQuad.push_back(t);
				}
			}
		}
	}

	pTexture->UnlockRect(0);
	Safe_Release(pTexture);

	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = 4 * (_ulong)vecQuad.size();
	m_dwTriCnt = m_dwVtxCnt / 2;
	m_dwFVF = FVF_TEX;

	m_dwIdxSize = sizeof(INDEX16);
	m_IdxFmt = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXTEX* pVertex = NULL;

	/// &pVertex : 버텍스 버퍼에 저장된 버텍스 중 첫 번째 버텍스
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	for (_ulong iVtxIdx = 0; iVtxIdx < m_dwVtxCnt; iVtxIdx += 4)
	{
		for (int i = 0; i < 4; ++i)
		{
			pVertex[iVtxIdx + i].vPosition = vecQuad[iVtxIdx / 4].vertex[i].vPosition;
			pVertex[iVtxIdx + i].vTexUV = vecQuad[iVtxIdx / 4].vertex[i].vTexUV;
		}
	}

	m_pVB->Unlock();

	INDEX16* pIndex = NULL;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	for (_ulong iIdxIdx = 0; iIdxIdx < m_dwTriCnt; iIdxIdx += 2)
	{
		int iVertexIdxOffset = 2 * iIdxIdx;
		for (int i = 0; i < 2; ++i)
		{
			pIndex[iIdxIdx + i]._0 = iVertexIdxOffset + vecQuad[iIdxIdx / 2].index[i]._0;
			pIndex[iIdxIdx + i]._1 = iVertexIdxOffset + vecQuad[iIdxIdx / 2].index[i]._1;
			pIndex[iIdxIdx + i]._2 = iVertexIdxOffset + vecQuad[iIdxIdx / 2].index[i]._2;
		}
	}

	m_pIB->Unlock();


	return S_OK;
}

void CVoxelBuffer::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CVoxelBuffer* CVoxelBuffer::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrTexturePath)
{
	CVoxelBuffer* pLaserBuffer = new CVoxelBuffer(pGraphicDev, wstrTexturePath);

	if (FAILED(pLaserBuffer->Ready_Buffer()))
	{
		Safe_Release(pLaserBuffer);
		MSG_BOX("CVoxelBuffer Create Failed");
		return nullptr;
	}

	return pLaserBuffer;
}

CComponent* CVoxelBuffer::Clone()
{
	return new CVoxelBuffer(*this);
}

TSideQuadInfo CVoxelBuffer::GetSideQuadInfo(int x, int y, EQuadDirection eDir, const D3DXIMAGE_INFO& tInfo)
{
	TSideQuadInfo t = {};
	float fOX = (float)tInfo.Width / 2.f;
	float fOY = (float)tInfo.Height / 2.f;

	/* 텍스쳐 u, v는 현재 픽셀과 같다 */
	/* 4 방향에 따른 서로 다른 쿼드 */
	switch (eDir)
	{
	case EQuadDirection::TOP:
		t.vertex[0].vPosition = { 0.5f,	-y + fOY, -x + fOX };
		t.vertex[1].vPosition = { 0.5f, -y + fOY, -(x + 1) + fOX };
		t.vertex[2].vPosition = { -0.5f, -y + fOY, -(x + 1) + fOX };
		t.vertex[3].vPosition = { -0.5f, -y + fOY, -x + fOX };

		t.index[0]._0 = 0;
		t.index[0]._1 = 1;
		t.index[0]._2 = 2;
		t.index[1]._0 = 0;
		t.index[1]._1 = 2;
		t.index[1]._2 = 3;
		break;
	case EQuadDirection::BOTTOM:
		t.vertex[0].vPosition = { 0.5f,	-(y + 1) + fOY, -x + fOX };
		t.vertex[1].vPosition = { 0.5f, -(y + 1) + fOY, -(x + 1) + fOX };
		t.vertex[2].vPosition = { -0.5f, -(y + 1) + fOY, -(x + 1) + fOX };
		t.vertex[3].vPosition = { -0.5f, -(y + 1) + fOY, -x + fOX };

		t.index[0]._0 = 0;
		t.index[0]._1 = 2;
		t.index[0]._2 = 1;
		t.index[1]._0 = 0;
		t.index[1]._1 = 3;
		t.index[1]._2 = 2;
		break;
	case EQuadDirection::LEFT:
		t.vertex[0].vPosition = { 0.5f,	-y + fOY, -x + fOX };
		t.vertex[1].vPosition = { -0.5f, -y + fOY, -x + fOX };
		t.vertex[2].vPosition = { -0.5f, -(y + 1) + fOY, -x + fOX };
		t.vertex[3].vPosition = { 0.5f, -(y + 1) + fOY, -x + fOX };

		t.index[0]._0 = 0;
		t.index[0]._1 = 1;
		t.index[0]._2 = 2;
		t.index[1]._0 = 0;
		t.index[1]._1 = 2;
		t.index[1]._2 = 3;
		break;
	case EQuadDirection::RIGHT:
		t.vertex[0].vPosition = { 0.5f,	-y + fOY, -(x + 1) + fOX };
		t.vertex[1].vPosition = { -0.5f, -y + fOY, -(x + 1) + fOX };
		t.vertex[2].vPosition = { -0.5f, -(y + 1) + fOY, -(x + 1) + fOX };
		t.vertex[3].vPosition = { 0.5f, -(y + 1) + fOY, -(x + 1) + fOX };

		t.index[0]._0 = 0;
		t.index[0]._1 = 2;
		t.index[0]._2 = 1;
		t.index[1]._0 = 0;
		t.index[1]._1 = 3;
		t.index[1]._2 = 2;
		break;
	default:
		break;
	}

	t.vertex[0].vTexUV = { ((float)x + 0.5f) / (float)tInfo.Width, ((float)y + 0.5f) / (float)tInfo.Height };
	t.vertex[1].vTexUV = { ((float)x + 0.5f) / (float)tInfo.Width, ((float)y + 0.5f) / (float)tInfo.Height };
	t.vertex[2].vTexUV = { ((float)x + 0.5f) / (float)tInfo.Width, ((float)y + 0.5f) / (float)tInfo.Height };
	t.vertex[3].vTexUV = { ((float)x + 0.5f) / (float)tInfo.Width, ((float)y + 0.5f) / (float)tInfo.Height };

	return t;
}

void CVoxelBuffer::Free()
{
	CVIBuffer::Free();
}
