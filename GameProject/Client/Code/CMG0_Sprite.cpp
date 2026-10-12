#include "pch.h"
#include "CMG0_Sprite.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CMiniGame.h"
#include "CMG0_Explosion.h"

CMG0_Sprite::CMG0_Sprite(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CMG0_Sprite::~CMG0_Sprite()
{
}

HRESULT CMG0_Sprite::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return CGameObject::Ready_GameObject();
}

_int CMG0_Sprite::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);

    return iExit;
}

void CMG0_Sprite::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMG0_Sprite::Render_GameObject()
{
    DWORD dwCull, dwLighting, dwFog, dwTexFactor;
    DWORD dwColorOp, dwColorArg1, dwColorArg2, dwAlphaOp, dwAlphaArg1, dwAlphaArg2;

    m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &dwCull);
    m_pGraphicDev->GetRenderState(D3DRS_LIGHTING, &dwLighting);
    m_pGraphicDev->GetRenderState(D3DRS_FOGENABLE, &dwFog);
    m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &dwTexFactor);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwColorArg1);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG2, &dwColorArg2);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAlphaOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAlphaArg1);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG2, &dwAlphaArg2);

    _float fFogStart = MG_FOG_START;
    _float fFogEnd = MG_FOG_END;
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_FOGENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
    m_pGraphicDev->SetRenderState(D3DRS_FOGCOLOR, MG_FOG_COLOR);
    m_pGraphicDev->SetRenderState(D3DRS_FOGSTART, *reinterpret_cast<DWORD*>(&fFogStart));
    m_pGraphicDev->SetRenderState(D3DRS_FOGEND, *reinterpret_cast<DWORD*>(&fFogEnd));
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, m_dwTint);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

    _matrix matBill, matScale, matTrans;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matBill);
    D3DXMatrixInverse(&matBill, nullptr, &matBill);
    matBill._41 = 0.f;
    matBill._42 = 0.f;
    matBill._43 = 0.f;

    D3DXMatrixScaling(&matScale, m_fScale, m_fScale, m_fScale);
    D3DXMatrixTranslation(&matTrans, m_vPos.x, m_vPos.y, m_vPos.z);
    _matrix matWorld = matScale * matBill * matTrans;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
    m_pTextureCom->Set_Texture(m_iFrame);
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, dwAlphaArg2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwAlphaArg1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwAlphaOp);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, dwColorArg2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwColorArg1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, dwColorOp);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwTexFactor);
    m_pGraphicDev->SetRenderState(D3DRS_FOGENABLE, dwFog);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, dwLighting);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, dwCull);
}

HRESULT CMG0_Sprite::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    return S_OK;
}

CMiniGame* CMG0_Sprite::Get_Scene()
{
    return dynamic_cast<CMiniGame*>(CManagement::GetInstance()->GetCurrentScene());
}

void CMG0_Sprite::Effect_Explode(const _vec3& vPos, _float fScale)
{
    CMiniGame* pScene = Get_Scene();
    if (nullptr == pScene)
        return;

    pScene->Add_GameObject(L"Explosion", CMG0_Explosion::Create(m_pGraphicDev, vPos, fScale));
}

void CMG0_Sprite::Free()
{
    CGameObject::Free();
}
