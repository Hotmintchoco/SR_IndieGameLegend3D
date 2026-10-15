#include "pch.h"
#include "COrthoSpriteTile.h"
#include "CProtoMgr.h"

COrthoSpriteTile::COrthoSpriteTile(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, int iTextureIdx, const TTileIdx& tIdx, int iGridX, int iGridZ, const float* pSharedTime)
    : CSpriteTile(pGraphicDev, iPositionIdx, iTextureIdx, true)
    , m_pSharedTime(pSharedTime)
    , m_iGridX(max(1, iGridX))
    , m_iGridZ(max(1, iGridZ))
{
    /* CTile 생성자는 기본 격자로 2D 인덱스를 계산하므로 미니 격자 인덱스로 덮어씀 */
    SetPositionIndex2D(tIdx);
}

COrthoSpriteTile::~COrthoSpriteTile()
{
}

HRESULT COrthoSpriteTile::Ready_GameObject()
{
    if (FAILED(CSpriteTile::Ready_GameObject()))
        return E_FAIL;

    if (FAILED(Add_BackgroundComponent()))
        return E_FAIL;

    /* 애니메이션 장 수 */
    m_iBackgroundFrameCount = max(1, (int)m_pBackgroundTextureCom->GetCount());

    return S_OK;
}

int COrthoSpriteTile::GetBackgroundIdx() const
{
    if (nullptr == m_pSharedTime || m_iBackgroundFrameCount <= 1) return 0;

    /* 타일마다 시간을 따로 쌓으면 꺼져 있던 동안(뚫린 칸) 멈춰 있다가 켜질 때 다른 칸과 장이 어긋나므로 공용 시간으로 계산 */
    const int iFrame = (int)(*m_pSharedTime / m_fBackgroundFrameInterval);
    return iFrame % m_iBackgroundFrameCount;
}

void COrthoSpriteTile::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    /* 텍스처 행렬 : 버퍼의 (u, v) 0~1을 배경 전체 중 이 칸의 조각으로 줄이고 옮김
       u' = u / 열 수 + 열 / 열 수, v' = v / 행 수 + 행 / 행 수
       2D UV는 (u, v, 1, 0)으로 확장되어 곱해지므로 이동량은 _31, _32에 넣음
       (CPlaneTex는 +z 쪽이 v 0, 미니 격자는 위쪽(+z)이 0행이라 뒤집을 필요 없음) */
    const TTileIdx tIdx = GetPositionIndex2D();

    _matrix matTex;
    D3DXMatrixIdentity(&matTex);
    matTex._11 = 1.f / (float)m_iGridX;
    matTex._22 = 1.f / (float)m_iGridZ;
    matTex._31 = (float)tIdx.iCol / (float)m_iGridX;
    matTex._32 = (float)tIdx.iRow / (float)m_iGridZ;

    m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &matTex);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

    /* 스테이지 1 : 칸 구분선. 이전 결과(배경)에 테두리 텍스처 색을 곱함 (흰색 = 그대로, 회색 = 어둡게)
       CTexture는 0번 스테이지에만 바인딩하므로 0번에 먼저 바인딩해 포인터를 얻고 1번으로 옮김 */
    m_pBorderTextureCom->Set_Texture(0);
    IDirect3DBaseTexture9* pBorderTexture = nullptr;
    m_pGraphicDev->GetTexture(0, &pBorderTexture);
    m_pGraphicDev->SetTexture(1, pBorderTexture);
    if (pBorderTexture)
        pBorderTexture->Release();	/* GetTexture가 참조 카운트를 올리므로 */

    /* 원래 UV(0~1)를 그대로 써서 타일마다 테두리 전체가 붙음 (텍스처 행렬은 스테이지별이라 1번은 꺼둠) */
    m_pGraphicDev->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
    m_pGraphicDev->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
    m_pGraphicDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
    m_pGraphicDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
    /* 알파는 스테이지 0 결과 그대로 */
    m_pGraphicDev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
    m_pGraphicDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

    /* 1픽셀 선이 번지거나 밉맵에서 사라지지 않도록 점 샘플링 (원래 값은 끝나고 복구) */
    DWORD dwMinFilter = 0, dwMagFilter = 0, dwMipFilter = 0;
    m_pGraphicDev->GetSamplerState(1, D3DSAMP_MINFILTER, &dwMinFilter);
    m_pGraphicDev->GetSamplerState(1, D3DSAMP_MAGFILTER, &dwMagFilter);
    m_pGraphicDev->GetSamplerState(1, D3DSAMP_MIPFILTER, &dwMipFilter);
    m_pGraphicDev->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pGraphicDev->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    m_pGraphicDev->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

    /* 스테이지 0 : 배경 조각 (위에서 테두리를 잠깐 0번에 바인딩했으므로 그 다음에 바인딩) */
    m_pBackgroundTextureCom->Set_Texture((_uint)GetBackgroundIdx());
    m_pBufferCom->Render_Buffer();

    /* 다음에 그려지는 오브젝트에 새지 않도록 원복 : 스테이지 1 끄기, 샘플러 복구, 스테이지 0 행렬 끄기 */
    m_pGraphicDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    m_pGraphicDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    m_pGraphicDev->SetTexture(1, nullptr);
    m_pGraphicDev->SetSamplerState(1, D3DSAMP_MINFILTER, dwMinFilter);
    m_pGraphicDev->SetSamplerState(1, D3DSAMP_MAGFILTER, dwMagFilter);
    m_pGraphicDev->SetSamplerState(1, D3DSAMP_MIPFILTER, dwMipFilter);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

HRESULT COrthoSpriteTile::Add_BackgroundComponent()
{
    CComponent* pComponent = nullptr;

    // Background Texture
    pComponent = m_pBackgroundTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_OrthoBackground_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_BackgroundTexture", pComponent });

    // Border Texture
    pComponent = m_pBorderTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Border_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_BorderTexture", pComponent });

    return S_OK;
}

COrthoSpriteTile* COrthoSpriteTile::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, int iTextureIdx, const TTileIdx& tIdx, int iGridX, int iGridZ, const float* pSharedTime)
{
    COrthoSpriteTile* pTile = new COrthoSpriteTile(pGraphicDev, iPositionIdx, iTextureIdx, tIdx, iGridX, iGridZ, pSharedTime);

    if (FAILED(pTile->Ready_GameObject()))
    {
        Safe_Release(pTile);
        MSG_BOX("COrthoSpriteTile Create Failed");
        return nullptr;
    }

    return pTile;
}

void COrthoSpriteTile::Free()
{
    CSpriteTile::Free();
}
