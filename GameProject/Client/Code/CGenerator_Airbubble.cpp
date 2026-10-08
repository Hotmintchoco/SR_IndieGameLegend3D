#include "pch.h"
#include "CGenerator_Airbubble.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CSphereCollider.h"
#include "CCollisionMgr.h"
#include "CRoomLayer.h"
#include "CManagement.h"
#include "CEffect.h"

CGenerator_Airbubble::CGenerator_Airbubble(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CGenerator_Airbubble::~CGenerator_Airbubble()
{
}

HRESULT CGenerator_Airbubble::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    return S_OK;
}

_int CGenerator_Airbubble::Update_GameObject(_float fTimeDelta)
{
    if (m_bStart == false)
    {
        m_bStart = true;
        _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
        m_vRoomCenterLocation = { GetCenterX(vPos.x), 0.f, GetCenterZ(vPos.z) };
    }
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    m_fElapsedTime += fTimeDelta;
    if (m_fElapsedTime > m_fEffectCoolTime)
    {
        m_fElapsedTime -= m_fEffectCoolTime;

        _float fRandX = _float(rand() % (15 - 4)) - 5.f;
        _float fRandZ = _float(rand() % (13 - 4)) - 4.f;

        _vec3 vPos = { m_vRoomCenterLocation.x + fRandX, 0.f, m_vRoomCenterLocation.z + fRandZ };
        CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");

        CGameObject* pGameObject = nullptr;
        pGameObject = CEffect::Create(m_pGraphicDev, CEffect::AIRBUBBLE, vPos);
        if (nullptr == pGameObject) return E_FAIL;
        if (FAILED(pLayer->Add_GameObject(L"Effect_Airbubble", pGameObject))) return E_FAIL;
    }

    return iExit;
}

void CGenerator_Airbubble::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CGenerator_Airbubble::Render_GameObject()
{
}

HRESULT CGenerator_Airbubble::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    return S_OK;
}

CGenerator_Airbubble* CGenerator_Airbubble::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CGenerator_Airbubble* pGenerator = new CGenerator_Airbubble(pGraphicDev);

    if (FAILED(pGenerator->Ready_GameObject()))
    {
        Safe_Release(pGenerator);
        MSG_BOX("CGenerator_Airbubble Create Failed");
        return nullptr;
    }

    return pGenerator;
}

void CGenerator_Airbubble::Free()
{
    CGameObject::Free();
}
