#include "pch.h"
#include "CEffect_YellowBox.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include <ctime>

CEffect_YellowBox::CEffect_YellowBox(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEffect(pGraphicDev), m_fElapsedLifeTime(0.f), m_fLifeTime(0.5f)
{
}

CEffect_YellowBox::CEffect_YellowBox(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3(&vYellowBox_Point)[4])
    : CEffect(pGraphicDev), m_fElapsedLifeTime(0.f), m_fLifeTime(0.5f)
{
}

CEffect_YellowBox::CEffect_YellowBox(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3(&vYellowBox_Point)[4], const _float& fLifeTime)
    : CEffect(pGraphicDev), m_fElapsedLifeTime(0.f), m_fLifeTime(fLifeTime)
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

    m_fLifeTime = 0.5f;
    float fScale = 0.5f * 0.5f * 0.5f;
    m_pTransformCom->Set_Scale(fScale, fScale, fScale);

    _vec3 vec3[4];
    vec3[0] = { -1.f, 1.f, 0.f };
    vec3[1] = { 1.f, 1.f, 0.f };
    vec3[2] = { 1.f, -1.f, 0.f };
    vec3[3] = { -1.f, -1.f, 0.f };

    D3DXCOLOR color[4];
    for (int i = 0; i < 4; ++i)
    {
        color[i] = { 1.f, 1.f, 0.f, 1.f };
    }

    static_cast<CRcColCustom*>(m_pBufferCom)->Set_Buffer(vec3, color);

    return S_OK;
}

_int CEffect_YellowBox::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CEffect::Update_GameObject(fTimeDelta);
    m_fElapsedLifeTime += fTimeDelta;
    if (m_fElapsedLifeTime > m_fLifeTime)
    {
        Set_Dead(true);
    }

    m_pTransformCom->Move_Pos(&m_vVelocity, 1.f, fTimeDelta);

    m_pTransformCom->Rotation(ROT_X, D3DXToRadian(10.f));
    m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(10.f));
    m_pTransformCom->Rotation(ROT_Z, D3DXToRadian(10.f));


    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CEffect_YellowBox::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CEffect::LateUpdate_GameObject(fTimeDelta);

    //CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
    //    ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

    //if (nullptr == pPlayerTransformCom)
    //    return;

    //_vec3   vPlayerPos;
    //_vec3   vPlayerLook;
    //pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);
    //pPlayerTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);

    //m_pTransformCom->LookAt_Player(&vPlayerPos, &vPlayerLook);

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
    pComponent = m_pBufferCom = dynamic_cast<CRcColCustom*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcColCustom"));

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

CEffect_YellowBox* CEffect_YellowBox::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CEffect_YellowBox* pEffect_YellowBox = new CEffect_YellowBox(pGraphicDev);

    if (FAILED(pEffect_YellowBox->Ready_GameObject()))
    {
        Safe_Release(pEffect_YellowBox);
        MSG_BOX("CEffect_YellowBox Create Failed");
        return nullptr;
    }
    pEffect_YellowBox->Set_Pos(vPos);
    return pEffect_YellowBox;
}

CEffect_YellowBox* CEffect_YellowBox::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vVelocity)
{
    CEffect_YellowBox* pEffect_YellowBox = new CEffect_YellowBox(pGraphicDev);

    if (FAILED(pEffect_YellowBox->Ready_GameObject()))
    {
        Safe_Release(pEffect_YellowBox);
        MSG_BOX("CEffect_YellowBox Create Failed");
        return nullptr;
    }
    pEffect_YellowBox->Set_Pos(vPos);
    pEffect_YellowBox->Set_Velocity(vVelocity);

    return pEffect_YellowBox;
}



void CEffect_YellowBox::Free()
{
    CEffect::Free();
}
