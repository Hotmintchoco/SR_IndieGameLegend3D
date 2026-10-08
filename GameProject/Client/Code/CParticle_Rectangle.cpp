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

    if (m_eType == SAND)
    {
        m_fFallingStartTime = (_float)(rand() % 128) / (256.f+128.f+64.f);
        _float fAngle = _float(rand() % 360);
        _vec3 vAngle = { 0.f,fAngle, 0.f };
        m_pTransformCom->Set_Angle(vAngle);
    }

    static_cast<CRcColCustom*>(m_pBufferCom)->Set_Buffer(vec3, color);

    return S_OK;
}

_int CParticle_Rectangle::Update_GameObject(_float fTimeDelta)
{
    _int    iExit = CParticle::Update_GameObject(fTimeDelta);
    
    if (m_fElapsedTime > m_fLifeTime)
    {
        Set_Dead(true);
    }

    if (m_eType == DEAD)
    {
        m_pTransformCom->Move_Pos(&m_vVelocity, 1.25f, fTimeDelta);

        m_pTransformCom->Rotation(ROT_X, D3DXToRadian(30.f * m_vVelocity.x));
        m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(30.f * m_vVelocity.y));
        m_pTransformCom->Rotation(ROT_Z, D3DXToRadian(30.f * m_vVelocity.z));
    }
    else if (m_eType == BULLET)
    {
        m_pTransformCom->Move_Pos(&m_vVelocity, 1.f, fTimeDelta);
        LookAtPlayer();
    }
    else if (m_eType == SAND)
    {
        if (m_fElapsedTime > m_fFallingStartTime)
        {
            m_pTransformCom->Move_Pos(&m_vVelocity, 2.f, fTimeDelta);
		    LookAtPlayer2();
        }
        _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
        if (vPos.y < 0.f)Set_Dead(true);
    }

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CParticle_Rectangle::LateUpdate_GameObject(_float fTimeDelta)
{
    CParticle::LateUpdate_GameObject(fTimeDelta);
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

CParticle_Rectangle* CParticle_Rectangle::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vVelocity, D3DXCOLOR eColor, _float fLifeTime)
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
    pEffect->Set_LifeTime(fLifeTime);

    return pEffect;
}

CParticle_Rectangle* CParticle_Rectangle::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vVelocity, _vec3 vScale, D3DXCOLOR eColor, _float fLifeTime)
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
    pEffect->Set_LifeTime(fLifeTime);
    pEffect->Set_Scale(vScale);

    return pEffect;
}

CParticle_Rectangle* CParticle_Rectangle::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vVelocity, _vec3 vScale, D3DXCOLOR eColor, _float fLifeTime, PARTICLE_RECT_TYPE eType)
{
    CParticle_Rectangle* pEffect = new CParticle_Rectangle(pGraphicDev);
    pEffect->Set_Velocity(vVelocity);
    pEffect->Set_Color(eColor);
    pEffect->Set_Type(eType);

    if (FAILED(pEffect->Ready_GameObject()))
    {
        Safe_Release(pEffect);
        MSG_BOX("CEffect_Rectangle Create Failed");
        return nullptr;
    }
    pEffect->Set_Pos(vPos);
    pEffect->Set_LifeTime(fLifeTime);
    pEffect->Set_Scale(vScale);

    return pEffect;
}

void CParticle_Rectangle::Free()
{
    CParticle::Free();
}
