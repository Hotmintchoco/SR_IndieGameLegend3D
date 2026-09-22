#include "CPlyTex.h"

#define TINYPLY_IMPLEMENTATION
#include "tinyply.h"

namespace
{
	// Blender는 정점 수에 따라 인덱스 타입을 uchar / ushort / uint 중에 고른다.
	WORD Get_Index(const unsigned char* pRaw, tinyply::Type eType, size_t iIndex)
	{
		switch (eType)
		{
		case tinyply::Type::UINT8:
			return reinterpret_cast<const unsigned char*>(pRaw)[iIndex];
		case tinyply::Type::UINT16:
			return reinterpret_cast<const unsigned short*>(pRaw)[iIndex];
		default: // UINT32 / INT32
			return static_cast<WORD>(reinterpret_cast<const unsigned int*>(pRaw)[iIndex]);
		}
	}
}

CPlyTex::CPlyTex()
{
}

CPlyTex::CPlyTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CPlyTex::CPlyTex(const CPlyTex& rhs)
	: CVIBuffer(rhs), m_vecIdx(rhs.m_vecIdx), m_vecVtx(rhs.m_vecVtx)
{
}

CPlyTex::~CPlyTex()
{
}

HRESULT CPlyTex::Ready_Buffer()
{
	MSG_BOX("[CPlyTex] 잘못된 함수 사용");
	return E_FAIL;
}

HRESULT CPlyTex::Ready_Buffer(const _tchar* szFilePath)
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
	m_VtxStructType = VTXSTRUCT_TEX;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXTEX* pVertex = nullptr;
	if (FAILED(m_pVB->Lock(0, 0, (void**)&pVertex, 0)))
		return E_FAIL;

	for (size_t i = 0; i < m_vecVtx.size(); ++i)
	{
		pVertex[i].vPosition = m_vecVtx[i].vPosition;
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
	}

	m_pIB->Unlock();

	return S_OK;
}

void CPlyTex::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CPlyTex* CPlyTex::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* szFilePath)
{
	CPlyTex* pPlyTex = new CPlyTex(pGraphicDev);

	if (FAILED(pPlyTex->Ready_Buffer(szFilePath)))
	{
		_tchar szMsg[MAX_PATH + 64] = L"";
		swprintf_s(szMsg, L"PlyTex Create Failed\n%s", szFilePath);
		MessageBox(NULL, szMsg, L"System Message", MB_OK);

		Safe_Release(pPlyTex);
		return nullptr;
	}

	return pPlyTex;
}

CComponent* CPlyTex::Clone()
{
	return new CPlyTex(*this);
}

HRESULT CPlyTex::Parse_Ply(const _tchar* szFilePath)
{
	/* Read File */

	ifstream Stream(szFilePath, ios::binary);
	if (!Stream)
	{
		MSG_BOX("[CPlyTex] Ply File Open Failed");
		return E_FAIL;
	}

	std::shared_ptr<tinyply::PlyData> pPos, pNor, pUV, pFace;

	try
	{
		tinyply::PlyFile PlyFile;
		PlyFile.parse_header(Stream);

		pPos = PlyFile.request_properties_from_element("vertex", { "x", "y", "z" });

		try { pNor = PlyFile.request_properties_from_element("vertex", { "nx", "ny", "nz" }); }
		catch (const std::exception&) {}

		try { pUV = PlyFile.request_properties_from_element("vertex", { "s", "t" }); }
		catch (const std::exception&) {}

		pFace = PlyFile.request_properties_from_element("face", { "vertex_indices" }, 3);

		PlyFile.read(Stream);
	}
	catch (const exception& e)
	{
		OutputDebugStringA("[CPlyTex] tinyply exception : ");
		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");
		return E_FAIL;
	}

	/* Validation */

	if (nullptr == pPos || nullptr == pFace)
	{
		MSG_BOX("[CPlyTex] Ply Data Read Failed");
		return E_FAIL;
	}

	const size_t iVtxCnt = pPos->count;

	if (iVtxCnt > 65535)
	{
		OutputDebugStringA("[CPlyTex] 정점이 65535개를 초과함. INDEX32가 필요.\n");
		return E_FAIL;
	}

	if (tinyply::Type::FLOAT32 != pPos->t)
	{
		OutputDebugStringA("[CPlyTex] 정점 좌표가 float32가 아님.\n");
		return E_FAIL;
	}

	switch (pFace->t)
	{
	case tinyply::Type::UINT8:
	case tinyply::Type::UINT16:
	case tinyply::Type::UINT32:
	case tinyply::Type::INT32:
		break;
	default:
		OutputDebugStringA("[CPlyTex] 지원하지 않는 인덱스 타입.\n");
		return E_FAIL;
	}

	const size_t iIdxStride = tinyply::PropertyTable[pFace->t].stride;
	if (pFace->buffer.size_bytes() != pFace->count * 3 * iIdxStride)
	{
		OutputDebugStringA("[CPlyTex] 삼각형이 아닌 면이 포함됨. "
			"Blender Export 시 Triangulated Mesh 옵션을 켤 것.\n");
		return E_FAIL;
	}

	/* Store Data */

	m_vecVtx.assign(iVtxCnt, VTXTEX{ { 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f } });

	const _float* pPosRaw = reinterpret_cast<const float*>(pPos->buffer.get());

	/* Revert Z to match DX coordinate convention */
	for (size_t i = 0; i < iVtxCnt; ++i)
	{
		m_vecVtx[i].vPosition = _vec3(pPosRaw[i * 3 + 0], pPosRaw[i * 3 + 1], -pPosRaw[i * 3 + 2]);
	}

	if (pNor && pNor->count == iVtxCnt && tinyply::Type::FLOAT32 == pNor->t)
	{
		const _float* pNorRaw = reinterpret_cast<const _float*>(pNor->buffer.get());

		for (size_t i = 0; i < iVtxCnt; ++i)
		{
			m_vecVtx[i].vNormal = _vec3(pNorRaw[i * 3 + 0], pNorRaw[i * 3 + 1], -pNorRaw[i * 3 + 2]);
		}
	}

	if (pUV && pUV->count == iVtxCnt && tinyply::Type::FLOAT32 == pUV->t)
	{
		const _float* pUVRaw = reinterpret_cast<const _float*>(pUV->buffer.get());

		for (size_t i = 0; i < iVtxCnt; ++i)
		{
			/* UV Coordinate Convention Calibration */
			m_vecVtx[i].vTexUV = _vec2(pUVRaw[i * 2 + 0], 1.f - pUVRaw[i * 2 + 1]);
		}
	}

	const unsigned char* pFaceRaw = pFace->buffer.get();

	m_vecIdx.clear();
	m_vecIdx.reserve(pFace->count * 3);

	for (size_t i = 0; i < pFace->count; ++i)
	{
		const WORD i0 = Get_Index(pFaceRaw, pFace->t, i * 3 + 0);
		const WORD i1 = Get_Index(pFaceRaw, pFace->t, i * 3 + 1);
		const WORD i2 = Get_Index(pFaceRaw, pFace->t, i * 3 + 2);

		/* Winding Convention Calibration */
		m_vecIdx.push_back(i0);
		m_vecIdx.push_back(i2);
		m_vecIdx.push_back(i1);
	}


	return m_vecVtx.empty() || m_vecIdx.empty() ? E_FAIL : S_OK;
}

void CPlyTex::Free()
{
	CVIBuffer::Free();
}
