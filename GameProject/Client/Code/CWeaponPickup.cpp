#include "pch.h"
#include "CWeaponPickup.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CTransform.h"
#include "CManagement.h"
#include "CSoundMgr.h"
#include "CWeaponSystem.h"

CWeaponPickup::CWeaponPickup(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pSpawner, EObjectType eWeaponType)
    : CItem(pGraphicDev), m_eWeaponType(eWeaponType)
{
    CTransform* pTransform = static_cast<CTransform*>(pSpawner->Get_Component(ID_DYNAMIC, L"Com_Transform"));
    pTransform->Get_Info(INFO_POS, &m_vSpawnPos);
}

CWeaponPickup::~CWeaponPickup()
{
}

HRESULT CWeaponPickup::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CItem::Ready_GameObject()))
        return E_FAIL;

    m_pTransformCom->Set_Pos(m_vSpawnPos + _vec3{ 0.f, 0.85f, 0.f });
    m_pTransformCom->Set_Scale(0.3f, 0.3f, 0.3f);

    m_iTextureIndex = (int)m_eWeaponType - (int)EObjectType::WEAPON_NONE - 1;

    return S_OK;
}

_int CWeaponPickup::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CItem::Update_GameObject(fTimeDelta);

    return iExit;
}

void CWeaponPickup::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CItem::LateUpdate_GameObject(fTimeDelta);
}

void CWeaponPickup::Render_GameObject()
{
    if (m_bBlinkStart == true && m_bVisible == false) return;
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture(m_iTextureIndex);
    m_pBufferCom->Render_Buffer();
}

HRESULT CWeaponPickup::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SkillTexture"));
    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CWeaponPickup::Consume()
{
    static_cast<CWeaponSystem*>(CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"WeaponSystem"))->AddWeapon(m_eWeaponType, L"Weapon_" + to_wstring((int)m_eWeaponType));

    Set_Dead(true);
}

CWeaponPickup* CWeaponPickup::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pSpawner, EObjectType eWeaponType)
{
    CWeaponPickup* pHeart = new CWeaponPickup(pGraphicDev, pSpawner, eWeaponType);

    if (FAILED(pHeart->Ready_GameObject()))
    {
        Safe_Release(pHeart);
        MSG_BOX("CWeaponPickup Create Failed");
        return nullptr;
    }

    return pHeart;
}

void CWeaponPickup::Free()
{
    CItem::Free();
}
