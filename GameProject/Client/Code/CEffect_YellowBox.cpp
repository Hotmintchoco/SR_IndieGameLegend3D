#include "pch.h"
#include "CEffect_YellowBox.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include <ctime>

CEffect_YellowBox::CEffect_YellowBox(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEffect(pGraphicDev)
{
}


CEffect_YellowBox::~CEffect_YellowBox()
{
}

HRESULT CEffect_YellowBox::Ready_GameObject()
{
    CEffect::Ready_GameObject();
    if (FAILED(Add_Component()))
        return E_FAIL;


    return S_OK;
}

_int CEffect_YellowBox::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CEffect::Update_GameObject(fTimeDelta);

    m_fFrame += 6.f * fTimeDelta;

    if (4.f <= m_fFrame)
        Set_Dead(true);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    return iExit;
}

void CEffect_YellowBox::LateUpdate_GameObject(const _float& fTimeDelta)
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

void CEffect_YellowBox::Render_GameObject()
{
    CEffect::Render_GameObject();

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pGraphicDev->SetTexture(0, nullptr);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CEffect_YellowBox::Add_Component()
{
    CComponent* pComponent = nullptr;


    // RcTex
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });


    return S_OK;
}


CEffect_YellowBox* CEffect_YellowBox::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CEffect_YellowBox* pEffect_YellowBox = new CEffect_YellowBox(pGraphicDev);

    if (FAILED(pEffect_YellowBox->Ready_GameObject()))
    {
        Safe_Release(pEffect_YellowBox);
        MSG_BOX("CEffect_YellowBox Create Failed");
        return nullptr;
    }

    return pEffect_YellowBox;
}


CEffect_YellowBox* CEffect_YellowBox::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale)
{
    CEffect_YellowBox* pEffect_YellowBox = new CEffect_YellowBox(pGraphicDev);

    if (FAILED(pEffect_YellowBox->Ready_GameObject()))
    {
        Safe_Release(pEffect_YellowBox);
        MSG_BOX("CEffect_YellowBox Create Failed");
        return nullptr;
    }
    pEffect_YellowBox->Set_Pos(vPos);
    pEffect_YellowBox->Set_Scale(vScale);
    return pEffect_YellowBox;
}


void CEffect_YellowBox::Free()
{
    CEffect::Free();
}
