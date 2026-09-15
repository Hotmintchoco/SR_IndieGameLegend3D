#include "pch.h"
#include "CAbstractFactory.h"
#include "CGraphicDev.h"
#include "CBreakableFrustum.h"
#include "CUnbreakableFrustum.h"
#include "CExplosiveFrustum.h"
#include "CSkull.h"
#include "CWorm.h"

IMPLEMENT_SINGLETON(CAbstractFactory);

CAbstractFactory::CAbstractFactory()
{
    LPDIRECT3DDEVICE9 pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();

    m_mapCreator = {
        {EObjectType::BREAKABLE_FRUSTUM,        [](LPDIRECT3DDEVICE9 pDevice) -> Engine::CGameObject* { return CBreakableFrustum::Create(pDevice); } },
        {EObjectType::UNBREAKABLE_FRUSTUM,      [](LPDIRECT3DDEVICE9 pDevice) -> Engine::CGameObject* { return CUnbreakableFrustum::Create(pDevice); } },
        {EObjectType::EXPLOSIVE_FRUSTUM,        [](LPDIRECT3DDEVICE9 pDevice) -> Engine::CGameObject* { return CExplosiveFrustum::Create(pDevice); } },
        {EObjectType::SKULL,                    [](LPDIRECT3DDEVICE9 pDevice) -> Engine::CGameObject* { return CSkull::Create(pDevice); } },
        {EObjectType::WORM,                     [](LPDIRECT3DDEVICE9 pDevice) -> Engine::CGameObject* { return CWorm::Create(pDevice); } },
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
