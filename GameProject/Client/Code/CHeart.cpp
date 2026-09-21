#include "pch.h"
#include "CHeart.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CGameStatusMgr.h"
#include "CTransform.h"
#include "CManagement.h"
#include "CPlayer.h"

CHeart::CHeart(LPDIRECT3DDEVICE9 pGraphicDev)
    : CItem(pGraphicDev)
{
}

CHeart::CHeart(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pSpawner)
    : CItem(pGraphicDev)
{
    CTransform* pTransform = static_cast<CTransform*>(pSpawner->Get_Component(ID_DYNAMIC, L"Com_Transform"));
    pTransform->Get_Info(INFO_POS, &m_vSpawnPos);
}

CHeart::~CHeart()
{
}

HRESULT CHeart::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CItem::Ready_GameObject()))
        return E_FAIL;

    //m_pTransformCom->Set_Pos(m_vSpawnPos + _vec3{ -0.2f, 0.1f, 0.f });
    m_pTransformCom->Set_Pos(m_vSpawnPos + _vec3{0.f, -m_vSpawnPos.y + 0.25f, 0.f });
    m_pTransformCom->Set_Scale(0.1f, 0.08f, 1.f);

    return S_OK;
}

_int CHeart::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CItem::Update_GameObject(fTimeDelta);

    return iExit;
}

void CHeart::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CItem::LateUpdate_GameObject(fTimeDelta);
}

void CHeart::Render_GameObject()
{
    if (m_bBlinkStart == true && m_bVisible == false) return;
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    
    m_pTextureCom->Set_Texture(0);
    m_pBufferCom->Render_Buffer();
}

HRESULT CHeart::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Item_Heart_Texture"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CHeart::Consume()
{
    CPlayer* pPlayer = static_cast<CPlayer*>(CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"Player"));
    pPlayer->GetItem(ITEMID::ITEM_HEAL);

    Set_Dead(true);
}

CHeart* CHeart::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CGameObject* pSpawner)
{
    CHeart* pHeart = new CHeart(pGraphicDev, pSpawner);

    if (FAILED(pHeart->Ready_GameObject()))
    {
        Safe_Release(pHeart);
        MSG_BOX("CHeart Create Failed");
        return nullptr;
    }

    return pHeart;
}

void CHeart::Free()
{
    CItem::Free();
}
