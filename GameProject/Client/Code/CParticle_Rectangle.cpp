#include "pch.h"
#include "CParticle_Rectangle.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include <ctime>

CParticle_Rectangle::CParticle_Rectangle(LPDIRECT3DDEVICE9 pGraphicDev)
    : CParticle(pGraphicDev)
{
}

CParticle_Rectangle::~CParticle_Rectangle()
{
}

HRESULT CParticle_Rectangle::Ready_GameObject()
{
    CParticle::Ready_GameObject();
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_fLifeTime = 1.f;
    float fScale = 0.5f * 0.5f * 0.5f * 0.75f;
    m_pTransformCom->Set_Scale(fScale, fScale, fScale);

    _vec3 vec3[4];
    vec3[0] = { -1.f, 1.f, 0.f };
    vec3[1] = { 1.f, 1.f, 0.f };
    vec3[2] = { 1.f, -1.f, 0.f };
    vec3[3] = { -1.f, -1.f, 0.f };

    D3DXCOLOR color[4];
    for (int i = 0; i < 4; ++i)
    {
        color[i] = m_eColor;
    }

    static_cast<CRcColCustom*>(m_pBufferCom)->Set_Buffer(vec3, color);

    return S_OK;
}

_int CParticle_Rectangle::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CParticle::Update_GameObject(fTimeDelta);
    
    if (m_fElapsedTime > m_fLifeTime)
    {
        Set_Dead(true);
    }

    m_pTransformCom->Move_Pos(&m_vVelocity, 1.25f, fTimeDelta);

    m_pTransformCom->Rotation(ROT_X, D3DXToRadian(30.f * m_vVelocity.x));
    m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(30.f * m_vVelocity.y));
    m_pTransformCom->Rotation(ROT_Z, D3DXToRadian(30.f * m_vVelocity.z));


    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CParticle_Rectangle::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CParticle::LateUpdate_GameObject(fTimeDelta);

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

void CParticle_Rectangle::Render_GameObject()
{
    CParticle::Render_GameObject();

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pGraphicDev->SetTexture(0, nullptr);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CParticle_Rectangle::Add_Component()
{
    CComponent* pComponent = nullptr;

    // RcTex
    pComponent = m_pBufferCom = dynamic_cast<CRcColCustom*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcColCustom"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });


    return S_OK;
}


CParticle_Rectangle* CParticle_Rectangle::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CParticle_Rectangle* pEffect_Rectangle = new CParticle_Rectangle(pGraphicDev);

    if (FAILED(pEffect_Rectangle->Ready_GameObject()))
    {
        Safe_Release(pEffect_Rectangle);
        MSG_BOX("CEffect_Rectangle Create Failed");
        return nullptr;
    }

    return pEffect_Rectangle;
}


CParticle_Rectangle* CParticle_Rectangle::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vVelocity, D3DXCOLOR eColor)
{
    CParticle_Rectangle* pEffect = new CParticle_Rectangle(pGraphicDev);
    pEffect->Set_Velocity(vVelocity);
    pEffect->Set_Color(eColor);

    if (FAILED(pEffect->Ready_GameObject()))
    {
        Safe_Release(pEffect);
        MSG_BOX("CEffect_Rectangle Create Failed");
        return nullptr;
    }
    pEffect->Set_Pos(vPos);

    return pEffect;
}



void CParticle_Rectangle::Free()
{
    CParticle::Free();
}
