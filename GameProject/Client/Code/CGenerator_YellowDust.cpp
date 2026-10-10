#include "pch.h"
#include "CGenerator_YellowDust.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CSphereCollider.h"
#include "CCollisionMgr.h"
#include "CRoomLayer.h"
#include "CManagement.h"
#include "CEffect.h"

CGenerator_YellowDust::CGenerator_YellowDust(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CGenerator_YellowDust::~CGenerator_YellowDust()
{
}

HRESULT CGenerator_YellowDust::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    return S_OK;
}

_int CGenerator_YellowDust::Update_GameObject(_float fTimeDelta)
{
    if (m_bStart == false)
    {
        m_bStart = true;
        _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
        m_vRoomCenterLocation = { GetCenterX(vPos.x), 0.f, GetCenterZ(vPos.z) };
        m_vEffectSpawnLocation = m_vRoomCenterLocation;

        //CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");

        //CGameObject* pGameObject = nullptr;
        //pGameObject = CEffect::Create(m_pGraphicDev, CEffect::YELLOWDUST_INIT, m_vRoomCenterLocation);
        //if (nullptr == pGameObject) return E_FAIL;
        //if (FAILED(pLayer->Add_GameObject(L"Effect_Snow", pGameObject))) return E_FAIL;
    }

    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    m_fElapsedTime += fTimeDelta;
    m_fElapsedTime2 += fTimeDelta;

    if (m_fElapsedTime2 >= m_fEffectCoolTime2)
    {
        m_fElapsedTime2 -= m_fEffectCoolTime2;

        CEffect::Set_Dir_YellowDust(m_vDir[m_iIndex]);
        _vec3 vDir = m_vDir[m_iIndex];
        vDir.y = 0.f;
        //vDir *= 2.f;
        m_vEffectSpawnLocation = m_vRoomCenterLocation - vDir * 0.5f;
        if (++m_iIndex == sizeof(m_vDir) / sizeof(m_vDir[0]))m_iIndex = 0;
    }

    if (m_fElapsedTime > m_fEffectCoolTime)
    {
        m_fElapsedTime -= m_fEffectCoolTime;

        CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");

        CGameObject* pGameObject = nullptr;
        pGameObject = CEffect::Create(m_pGraphicDev, CEffect::YELLOWDUST, m_vEffectSpawnLocation);
        if (nullptr == pGameObject) return E_FAIL;
        if (FAILED(pLayer->Add_GameObject(L"Effect_Snow", pGameObject))) return E_FAIL;
    }
    return iExit;
}

void CGenerator_YellowDust::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CGenerator_YellowDust::Render_GameObject()
{
}

HRESULT CGenerator_YellowDust::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    return S_OK;
}

CGenerator_YellowDust* CGenerator_YellowDust::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CGenerator_YellowDust* pGenerator = new CGenerator_YellowDust(pGraphicDev);

    if (FAILED(pGenerator->Ready_GameObject()))
    {
        Safe_Release(pGenerator);
        MSG_BOX("CGenerator_YellowDust Create Failed");
        return nullptr;
    }

    return pGenerator;
}

void CGenerator_YellowDust::Free()
{
    CGameObject::Free();
}
