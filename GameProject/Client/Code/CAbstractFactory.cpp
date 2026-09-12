#include "pch.h"
#include "CAbstractFactory.h"
#include "CGraphicDev.h"
#include "CFrustum.h"
#include "CMonster.h"

CAbstractFactory::CAbstractFactory()
{
    LPDIRECT3DDEVICE9 pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();

    m_mapCreator = {
        {EObjectType::BREAKABLE_FRUSTUM,        [](LPDIRECT3DDEVICE9 pDevice) -> Engine::CGameObject* { return CFrustum::Create(pDevice); } },
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
