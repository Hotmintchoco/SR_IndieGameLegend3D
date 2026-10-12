#include "pch.h"
#include "CMG0_Intro.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CMiniGame.h"

CMG0_Intro::CMG0_Intro(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CMG0_Intro::~CMG0_Intro()
{
}

HRESULT CMG0_Intro::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return CGameObject::Ready_GameObject();
}

_int CMG0_Intro::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CMiniGame* pScene = dynamic_cast<CMiniGame*>(CManagement::GetInstance()->GetCurrentScene());
    if (pScene)
        m_fTime = pScene->Get_SceneTime();

    if (m_fTime >= MG_INTRO_TOTAL_TIME)
    {
        Set_Dead(true);
        return iExit;
    }

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return iExit;
}

void CMG0_Intro::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMG0_Intro::Render_GameObject()
{
    DWORD dwCull, dwLighting, dwZEnable, dwAlphaTest, dwTexFactor;
    DWORD dwColorOp, dwColorArg2, dwAlphaOp, dwAlphaArg2;

    m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &dwCull);
    m_pGraphicDev->GetRenderState(D3DRS_LIGHTING, &dwLighting);
    m_pGraphicDev->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
    m_pGraphicDev->GetRenderState(D3DRS_ALPHATESTENABLE, &dwAlphaTest);
    m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &dwTexFactor);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG2, &dwColorArg2);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAlphaOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG2, &dwAlphaArg2);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_pGraphicDev->SetTexture(0, nullptr);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

    const _float fHalfX = WINCX * 0.5f;
    const _float fHalfY = WINCY * 0.5f;
    const _float fLineHalfY = 2.f;

    _float fTime = m_fTime;

    if (fTime < MG_INTRO_WHITE_TIME)
    {
        Render_Rect(fHalfX, fHalfY, 0xFFFFFFFF);
    }
    else if ((fTime -= MG_INTRO_WHITE_TIME) < MG_INTRO_SHRINK_Y_TIME)
    {
        _float fRate = fTime / MG_INTRO_SHRINK_Y_TIME;
        _float fEase = 1.f - (1.f - fRate) * (1.f - fRate);

        Render_Rect(fHalfX, fHalfY, 0xFF000000);
        Render_Rect(fHalfX, fHalfY + (fLineHalfY - fHalfY) * fEase, 0xFFFFFFFF);
    }
    else if ((fTime -= MG_INTRO_SHRINK_Y_TIME) < MG_INTRO_SHRINK_X_TIME)
    {
        _float fRate = fTime / MG_INTRO_SHRINK_X_TIME;

        Render_Rect(fHalfX, fHalfY, 0xFF000000);
        Render_Rect(fHalfX * (1.f - fRate * fRate), fLineHalfY, 0xFFFFFFFF);
    }
    else if ((fTime -= MG_INTRO_SHRINK_X_TIME) < MG_INTRO_BLACK_TIME)
    {
        Render_Rect(fHalfX, fHalfY, 0xFF000000);
    }
    else
    {
        fTime -= MG_INTRO_BLACK_TIME;

        _float fRate = max(0.f, min(1.f, fTime / MG_INTRO_FADE_TIME));

        Render_Rect(fHalfX, fHalfY, D3DCOLOR_ARGB((_int)(255.f * (1.f - fRate)), 0, 0, 0));
    }

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, dwAlphaArg2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwAlphaOp);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, dwColorArg2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, dwColorOp);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwTexFactor);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, dwAlphaTest);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, dwZEnable);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, dwLighting);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, dwCull);
}

void CMG0_Intro::Render_Rect(_float fHalfX, _float fHalfY, DWORD dwColor)
{
    _matrix matScale, matTrans;
    D3DXMatrixScaling(&matScale, fHalfX, fHalfY, 1.f);
    D3DXMatrixTranslation(&matTrans, 0.f, 0.f, 0.5f);
    _matrix matWorld = matScale * matTrans;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwColor);
    m_pBufferCom->Render_Buffer();
}

HRESULT CMG0_Intro::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    return S_OK;
}

CMG0_Intro* CMG0_Intro::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMG0_Intro* pIntro = new CMG0_Intro(pGraphicDev);

    if (FAILED(pIntro->Ready_GameObject()))
    {
        Safe_Release(pIntro);
        MSG_BOX("CMG0_Intro Create Failed");
        return nullptr;
    }

    return pIntro;
}

void CMG0_Intro::Free()
{
    CGameObject::Free();
}
