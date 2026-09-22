#include "pch.h"
#include "CSmallExplode.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include <ctime>

CSmallExplode::CSmallExplode(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEffect(pGraphicDev)
{
}


CSmallExplode::~CSmallExplode()
{
}

HRESULT CSmallExplode::Ready_GameObject()
{
    CEffect::Ready_GameObject();
    if (FAILED(Add_Component()))
        return E_FAIL;


    return S_OK;
}

_int CSmallExplode::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CEffect::Update_GameObject(fTimeDelta);

    m_fFrame += 6.f * fTimeDelta;

    if (4.f <= m_fFrame)
        Set_Dead(true);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);

    return iExit;
}

void CSmallExplode::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CEffect::LateUpdate_GameObject(fTimeDelta);

    CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
        ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

    if (nullptr == pPlayerTransformCom)
        return;

    _vec3   vPlayerPos;
    _vec3   vPlayerLook;
    pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);
    pPlayerTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);

    m_pTransformCom->LookAt_Player(&vPlayerPos, &vPlayerLook);

}

void CSmallExplode::Render_GameObject()
{
    CEffect::Render_GameObject();

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture((_uint)m_fFrame);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CSmallExplode::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_smallexplodeTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // RcTex
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });


    return S_OK;
}


CSmallExplode* CSmallExplode::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CSmallExplode* pSmallExplode = new CSmallExplode(pGraphicDev);

    if (FAILED(pSmallExplode->Ready_GameObject()))
    {
        Safe_Release(pSmallExplode);
        MSG_BOX("CSmallExplode Create Failed");
        return nullptr;
    }

    return pSmallExplode;
}


CSmallExplode* CSmallExplode::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale)
{
    CSmallExplode* pSmallExplode = new CSmallExplode(pGraphicDev);
    
    if (FAILED(pSmallExplode->Ready_GameObject()))
    {
        Safe_Release(pSmallExplode);
        MSG_BOX("CSmallExplode Create Failed");
        return nullptr;
    }
    pSmallExplode->Set_Pos(vPos);
    pSmallExplode->Set_Scale(vScale);
    return pSmallExplode;
}


void CSmallExplode::Free()
{
    CEffect::Free();
}
