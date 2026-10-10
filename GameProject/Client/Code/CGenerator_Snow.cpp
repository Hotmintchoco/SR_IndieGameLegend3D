#include "pch.h"
#include "CGenerator_Snow.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CSphereCollider.h"
#include "CCollisionMgr.h"
#include "CRoomLayer.h"
#include "CManagement.h"
#include "CEffect.h"

CGenerator_Snow::CGenerator_Snow(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CGenerator_Snow::~CGenerator_Snow()
{
}

HRESULT CGenerator_Snow::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    return S_OK;
}

_int CGenerator_Snow::Update_GameObject(_float fTimeDelta)
{
    if (m_bStart == false)
    {
        m_bStart = true;
        _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
        m_vRoomCenterLocation = { GetCenterX(vPos.x), 0.f, GetCenterZ(vPos.z) };

        CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");

        CGameObject* pGameObject = nullptr;
        pGameObject = CEffect::Create(m_pGraphicDev, CEffect::SNOW_INIT, m_vRoomCenterLocation);
        if (nullptr == pGameObject) return E_FAIL;
        if (FAILED(pLayer->Add_GameObject(L"Effect_Snow", pGameObject))) return E_FAIL;
    }

    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    m_fElapsedTime += fTimeDelta;
    if (m_fElapsedTime > m_fEffectCoolTime)
    {
        m_fElapsedTime -= m_fEffectCoolTime;

        CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");

        CGameObject* pGameObject = nullptr;
        pGameObject = CEffect::Create(m_pGraphicDev, CEffect::SNOW, m_vRoomCenterLocation);
        if (nullptr == pGameObject) return E_FAIL;
        if (FAILED(pLayer->Add_GameObject(L"Effect_Snow", pGameObject))) return E_FAIL;
    }

    return iExit;
}

void CGenerator_Snow::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CGenerator_Snow::Render_GameObject()
{
}

HRESULT CGenerator_Snow::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    return S_OK;
}

CGenerator_Snow* CGenerator_Snow::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CGenerator_Snow* pGenerator = new CGenerator_Snow(pGraphicDev);

    if (FAILED(pGenerator->Ready_GameObject()))
    {
        Safe_Release(pGenerator);
        MSG_BOX("CGenerator_Snow Create Failed");
        return nullptr;
    }

    return pGenerator;
}

void CGenerator_Snow::Free()
{
    CGameObject::Free();
}
