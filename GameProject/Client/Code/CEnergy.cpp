#include "pch.h"
#include "CEnergy.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CGameStatusMgr.h"
#include "CTransform.h"
#include "CWeapon.h"
#include "CManagement.h"
#include "CSoundMgr.h"

CEnergy::CEnergy(LPDIRECT3DDEVICE9 pGraphicDev)
    : CItem(pGraphicDev)
{
}

CEnergy::CEnergy(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pSpawner)
    : CItem(pGraphicDev)
{
    CTransform* pTransform = static_cast<CTransform*>(pSpawner->Get_Component(ID_DYNAMIC, L"Com_Transform"));
    pTransform->Get_Info(INFO_POS, &m_vSpawnPos);
}

CEnergy::~CEnergy()
{
}

HRESULT CEnergy::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CItem::Ready_GameObject()))
        return E_FAIL;

    m_pTransformCom->Set_Scale(0.20f, 0.20f, 1.f);
    m_pTransformCom->Set_Pos(m_vSpawnPos + _vec3{ 0.f, -m_vSpawnPos.y + 0.45f, 0.f });

    return S_OK;
}

_int CEnergy::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CItem::Update_GameObject(fTimeDelta);

    m_fFrame += fTimeDelta * 6.f;
    if (m_fFrame > 8.f)
        m_fFrame = 0.f;

    return iExit;
}

void CEnergy::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CItem::LateUpdate_GameObject(fTimeDelta);
}

void CEnergy::Render_GameObject()
{
    if (m_bBlinkStart == true && m_bVisible == false) return;
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture((_uint)m_fFrame);
    m_pBufferCom->Render_Buffer();
}

HRESULT CEnergy::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Item_Energy_Texture"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CEnergy::Consume()
{
    CWeapon* pGun = static_cast<CWeapon*>(CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"Gun"));
    pGun->GainEnergy();

    Set_Dead(true);

    CSoundMgr::GetInstance()->PlaySFX(L"sfxEnergy.wav");
}

CEnergy* CEnergy::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pSpawner)
{
    CEnergy* pEnergy = new CEnergy(pGraphicDev, pSpawner);

    if (FAILED(pEnergy->Ready_GameObject()))
    {
        Safe_Release(pEnergy);
        MSG_BOX("CEnergy Create Failed");
        return nullptr;
    }

    return pEnergy;
}

void CEnergy::Free()
{
    CItem::Free();
}
