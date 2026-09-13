#include "pch.h"
#include "CAbstractFactory.h"
#include "CGraphicDev.h"
#include "CBreakableFrustum.h"
#include "CUnbreakableFrustum.h"
#include "CExplosiveFrustum.h"
#include "CMonster.h"

CAbstractFactory::CAbstractFactory()
{
    LPDIRECT3DDEVICE9 pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();

    m_mapCreator = {
        {EObjectType::BREAKABLE_FRUSTUM,        [](LPDIRECT3DDEVICE9 pDevice) -> Engine::CGameObject* { return CBreakableFrustum::Create(pDevice); } },
        {EObjectType::UNBREAKABLE_FRUSTUM,      [](LPDIRECT3DDEVICE9 pDevice) -> Engine::CGameObject* { return CUnbreakableFrustum::Create(pDevice); } },
        {EObjectType::EXPLOSIVE_FRUSTUM,        [](LPDIRECT3DDEVICE9 pDevice) -> Engine::CGameObject* { return CExplosiveFrustum::Create(pDevice); } },
        {EObjectType::MONSTER,                  [](LPDIRECT3DDEVICE9 pDevice) -> Engine::CGameObject* { return CMonster::Create(pDevice); } },
    };
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
