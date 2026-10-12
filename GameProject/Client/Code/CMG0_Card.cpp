#include "pch.h"
#include "CMG0_Card.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CMG0_Card::CMG0_Card(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CMG0_Card::~CMG0_Card()
{
}

HRESULT CMG0_Card::Ready_GameObject(MINIGAME0::MGCARD eCard, _int iSlot, Engine::CTexture* pTexture)
{
    if (FAILED(Add_Component(pTexture)))
        return E_FAIL;

    m_eCard = eCard;
    m_vPos.x = (iSlot - (MG_CARD_HAND_COUNT - 1) * 0.5f) * MG_CARD_UI_GAP_X;
    m_vPos.y = WINCY * 0.5f - MG_CARD_UI_CENTER_Y;

    return CGameObject::Ready_GameObject();
}

_int CMG0_Card::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return iExit;
}

void CMG0_Card::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMG0_Card::Render_GameObject()
{
    DWORD dwCull, dwLighting, dwZEnable, dwAlphaTest, dwTexFactor, dwMagFilter, dwMinFilter;
    DWORD dwColorOp, dwColorArg1, dwColorArg2, dwAlphaOp, dwAlphaArg1, dwAlphaArg2;

    m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &dwCull);
    m_pGraphicDev->GetRenderState(D3DRS_LIGHTING, &dwLighting);
    m_pGraphicDev->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
    m_pGraphicDev->GetRenderState(D3DRS_ALPHATESTENABLE, &dwAlphaTest);
    m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &dwTexFactor);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwColorArg1);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG2, &dwColorArg2);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAlphaOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAlphaArg1);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG2, &dwAlphaArg2);
    m_pGraphicDev->GetSamplerState(0, D3DSAMP_MAGFILTER, &dwMagFilter);
    m_pGraphicDev->GetSamplerState(0, D3DSAMP_MINFILTER, &dwMinFilter);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

    _float fRaise = m_bSelected ? MG_CARD_UI_SELECT_RAISE : 0.f;

    _matrix matScale, matTrans;
    D3DXMatrixScaling(&matScale, MG_CARD_UI_HALF_X, MG_CARD_UI_HALF_Y, 1.f);
    D3DXMatrixTranslation(&matTrans, m_vPos.x, m_vPos.y + fRaise, 0.5f);
    _matrix matWorld = matScale * matTrans;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, m_bUsable ? 0xFFFFFFFF : 0xFF505050);
    m_pTextureCom->Set_Texture(0);
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, dwMinFilter);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, dwMagFilter);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, dwAlphaArg2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwAlphaArg1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwAlphaOp);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, dwColorArg2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwColorArg1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, dwColorOp);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwTexFactor);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, dwAlphaTest);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, dwZEnable);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, dwLighting);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, dwCull);
}

HRESULT CMG0_Card::Add_Component(Engine::CTexture* pTexture)
{
    CComponent* pComponent = nullptr;

    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = pTexture;

    if (nullptr == pComponent)
        return E_FAIL;

    pComponent->AddRef();
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CMG0_Card* CMG0_Card::Create(LPDIRECT3DDEVICE9 pGraphicDev, MINIGAME0::MGCARD eCard, _int iSlot, Engine::CTexture* pTexture)
{
    CMG0_Card* pCard = new CMG0_Card(pGraphicDev);

    if (FAILED(pCard->Ready_GameObject(eCard, iSlot, pTexture)))
    {
        Safe_Release(pCard);
        MSG_BOX("CMG0_Card Create Failed");
        return nullptr;
    }

    return pCard;
}

void CMG0_Card::Free()
{
    CGameObject::Free();
}
