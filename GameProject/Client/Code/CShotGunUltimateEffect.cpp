#include "pch.h"
#include "CShotGunUltimateEffect.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CPlayer.h"

CShotGunUltimateEffect::CShotGunUltimateEffect(LPDIRECT3DDEVICE9 pGraphicDev, CPlayer* pTarget)
    : CGameObject(pGraphicDev), m_pTarget(pTarget)
{
}

CShotGunUltimateEffect::~CShotGunUltimateEffect()
{
}

HRESULT CShotGunUltimateEffect::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    m_pFloorTransform->Set_Scale(_vec3{ 4.f, 1.f, 4.f });
    m_pWallTransform->Set_Scale(_vec3{ 1.5f, 1.5f, 1.5f });

    m_fTimeAfterBirth = 0.f;
    m_iFloorIndex = 0;
    m_iWallIndex = 0;
    m_iFloorFrameCount = m_pFloorTexture->GetCount();
    m_iWallFrameCount = m_pWallTexture->GetCount();

    SyncTransformToTarget();

    return S_OK;
}

_int CShotGunUltimateEffect::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    m_fTimeAfterBirth += fTimeDelta;

    if (m_fTimeAfterBirth >= m_fLifeTime)
    {
        Set_Dead(true);
        return 0;
    }

    SyncTransformToTarget();

    UpdateAnimation();

    _vec3 vPos = m_pFloorTransform->Get_Info_Value(INFO_POS);
    Compute_ViewZ(&vPos);

    return iExit;
}

void CShotGunUltimateEffect::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CShotGunUltimateEffect::Render_GameObject()
{
    // 바닥
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pFloorTransform->Get_World());
    m_pFloorTexture->Set_Texture(m_iFloorIndex);
    m_pFloorBuffer->Render_Buffer();

    // 벽
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pWallTransform->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    
    m_pWallTexture->Set_Texture(m_iWallIndex);
    m_pWallBuffer->Render_Buffer();
    
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CShotGunUltimateEffect::SyncTransformToTarget()
{
    if (!m_pTarget) return;

    _vec3 vPos = m_pTarget->GetTransform()->Get_Info_Value(INFO_POS);
    m_pFloorTransform->Set_Pos(vPos + _vec3{0.f, -vPos.y + 0.03f, 0.f});
    m_pWallTransform->Set_Pos(vPos);
}

void CShotGunUltimateEffect::UpdateAnimation()
{
    if (m_iFloorFrameCount > 0)
        m_iFloorIndex = static_cast<int>(m_fTimeAfterBirth / m_fFloorFrameInterval) % m_iFloorFrameCount;

    if (m_iWallFrameCount > 0)
        m_iWallIndex = static_cast<int>(m_fTimeAfterBirth / m_fWallFrameInterval) % m_iWallFrameCount;
}

HRESULT CShotGunUltimateEffect::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pFloorTransform = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_FloorTransform", pComponent });

    // Transform
    pComponent = m_pWallTransform = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_WallTransform", pComponent });

    // Floor Buffer
    pComponent = m_pFloorBuffer = dynamic_cast<CPlaneTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_PlaneTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_FloorBuffer", pComponent });

    // Wall Buffer
    pComponent = m_pWallBuffer = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Ultimate_SG_Wall_Cylinder_Buffer"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_WallBuffer", pComponent });

    // Floor Texture
    pComponent = m_pFloorTexture = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Ultimate_SG_Floor_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_FloorTexture", pComponent });

    // Wall Texture
    pComponent = m_pWallTexture = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Ultimate_SG_Wall_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_WallTexture", pComponent });

    return S_OK;
}

CShotGunUltimateEffect* CShotGunUltimateEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, CPlayer* pTarget)
{
    CShotGunUltimateEffect* pEffect = new CShotGunUltimateEffect(pGraphicDev, pTarget);

    if (FAILED(pEffect->Ready_GameObject()))
    {
        Safe_Release(pEffect);
        MSG_BOX("CShotGunUltimateEffect Create Failed");
        return nullptr;
    }

    return pEffect;
}

void CShotGunUltimateEffect::Free()
{
    CGameObject::Free();
}