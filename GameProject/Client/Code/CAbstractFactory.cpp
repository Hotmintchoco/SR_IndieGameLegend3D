#include "pch.h"
#include "CAbstractFactory.h"
#include "CGraphicDev.h"
#include "CRandomMgr.h"
#include "CBreakableFrustum.h"
#include "CUnbreakableFrustum.h"
#include "CExplosiveFrustum.h"
#include "CSkull.h"
#include "CSpeyeder.h"
#include "CBoss1.h"
#include "CHeart.h"
#include "CEnergy.h"
#include "CGem.h"
#include "CMagmamouth.h"
#include "CGameMachine.h"
#include "CRapidGun.h"
#include "CShotGun.h"

IMPLEMENT_SINGLETON(CAbstractFactory);

CAbstractFactory::CAbstractFactory()
{
    m_mapCreator = {
        {EObjectType::BREAKABLE_FRUSTUM,        [](const TCreateDesc& t) -> Engine::CGameObject* { return CBreakableFrustum::Create(t.pDevice); } },
        {EObjectType::UNBREAKABLE_FRUSTUM,      [](const TCreateDesc& t) -> Engine::CGameObject* { return CUnbreakableFrustum::Create(t.pDevice); } },
        {EObjectType::EXPLOSIVE_FRUSTUM,        [](const TCreateDesc& t) -> Engine::CGameObject* { return CExplosiveFrustum::Create(t.pDevice); } },
        {EObjectType::GAME_MACHINE,             [](const TCreateDesc& t) -> Engine::CGameObject* { return CGameMachine::Create(t.pDevice); } },

        {EObjectType::Skull,                    [](const TCreateDesc& t) -> Engine::CGameObject* { return CSkull::Create(t.pDevice); } },
        {EObjectType::Boss1,                   [](const TCreateDesc& t) -> Engine::CGameObject* { return CBoss1::Create(t.pDevice); } },
        {EObjectType::Speyeder,                [](const TCreateDesc& t) -> Engine::CGameObject* { return CSpeyeder::Create(t.pDevice); } },
        {EObjectType::Magmamouth,                [](const TCreateDesc& t) -> Engine::CGameObject* { return CMagmamouth::Create(t.pDevice); } },

        {EObjectType::ITEM_HEART,               [](const TCreateDesc& t) -> Engine::CGameObject* { return CHeart::Create(t.pDevice, t.pSpawner); } },
        {EObjectType::ITEM_ENERGY,              [](const TCreateDesc& t) -> Engine::CGameObject* { return CEnergy::Create(t.pDevice, t.pSpawner); } },
        {EObjectType::ITEM_GEM,                 [](const TCreateDesc& t) -> Engine::CGameObject* { return CGem::Create(t.pDevice, t.pSpawner); } },

        {EObjectType::WEAPON_DEFAULT,           [](const TCreateDesc& t) -> Engine::CGameObject* { return CRapidGun::Create(t.pDevice); } },
        {EObjectType::WEAPON_SHOTGUN,           [](const TCreateDesc& t) -> Engine::CGameObject* { return CShotGun::Create(t.pDevice); } },
    };
}

CAbstractFactory::~CAbstractFactory()
{
    Free();
}

Engine::CGameObject* CAbstractFactory::Create(EObjectType eType) const
{
    TCreateDesc t
    {
        CGraphicDev::GetInstance()->Get_GraphicDev(),
        nullptr,
    };

    Engine::CGameObject* pObject = m_mapCreator.at(eType)(t);

    return pObject;
}

Engine::CGameObject* CAbstractFactory::CreateRandomItem(CGameObject* pSpawner) const
{
    const int iBegin = (int)EObjectType::ITEM_NONE + 1;
    const int iEnd = (int)EObjectType::ITEM_MAX - 1;

    EObjectType eType = (EObjectType)CRandomMgr::GetInstance()->GetRandomValue<int>(iBegin, iEnd);

    TCreateDesc t
    {
        CGraphicDev::GetInstance()->Get_GraphicDev(),
        pSpawner,
    };

    LPDIRECT3DDEVICE9 pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();

    Engine::CGameObject* pObject = m_mapCreator.at(eType)(t);

    return pObject;
}

CWeapon* CAbstractFactory::CraeteWeapon(EObjectType eType) const
{
    if ((int)eType <= (int)EObjectType::WEAPON_NONE || (int)eType >= (int)EObjectType::WEAPON_MAX) return nullptr;

    TCreateDesc t
    {
        CGraphicDev::GetInstance()->Get_GraphicDev(),
    };

    LPDIRECT3DDEVICE9 pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();

    Engine::CGameObject* pObject = m_mapCreator.at(eType)(t);

    return static_cast<CWeapon*>(pObject);
}

void CAbstractFactory::Free()
{
}
