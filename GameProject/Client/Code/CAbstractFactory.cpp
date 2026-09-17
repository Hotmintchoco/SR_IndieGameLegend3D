#include "pch.h"
#include "CAbstractFactory.h"
#include "CGraphicDev.h"
#include "CBreakableFrustum.h"
#include "CUnbreakableFrustum.h"
#include "CExplosiveFrustum.h"
#include "CSkull.h"
#include "CSpeyeder.h"
#include "CBoss1.h"

IMPLEMENT_SINGLETON(CAbstractFactory);

CAbstractFactory::CAbstractFactory()
{
    LPDIRECT3DDEVICE9 pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();

    m_mapCreator = {
        {EObjectType::BREAKABLE_FRUSTUM,        [](LPDIRECT3DDEVICE9 pDevice) -> Engine::CGameObject* { return CBreakableFrustum::Create(pDevice); } },
        {EObjectType::UNBREAKABLE_FRUSTUM,      [](LPDIRECT3DDEVICE9 pDevice) -> Engine::CGameObject* { return CUnbreakableFrustum::Create(pDevice); } },
        {EObjectType::EXPLOSIVE_FRUSTUM,        [](LPDIRECT3DDEVICE9 pDevice) -> Engine::CGameObject* { return CExplosiveFrustum::Create(pDevice); } },
        
        {EObjectType::Skull,                    [](LPDIRECT3DDEVICE9 pDevice) -> Engine::CGameObject* { return CSkull::Create(pDevice); } },
        {EObjectType::Boss1,                   [](LPDIRECT3DDEVICE9 pDevice) -> Engine::CGameObject* { return CBoss1::Create(pDevice); } },
        {EObjectType::Speyeder,                [](LPDIRECT3DDEVICE9 pDevice) -> Engine::CGameObject* { return CSpeyeder::Create(pDevice); } },
    };
}

CAbstractFactory::~CAbstractFactory()
{
    Free();
}

Engine::CGameObject* CAbstractFactory::Create(EObjectType eType) const
{
    LPDIRECT3DDEVICE9 pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();

    Engine::CGameObject* pObject = m_mapCreator.at(eType)(pDevice);
    
    return pObject;
}

void CAbstractFactory::Free()
{
}
