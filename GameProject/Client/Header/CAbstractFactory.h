#pragma once

#include "CBase.h"
#include "Client_Struct.h"

namespace Engine
{
	class CGameObject;
}

struct TCreateDesc
{
	LPDIRECT3DDEVICE9 pDevice = nullptr;
	CGameObject* pSpawner = nullptr;
};

class CWeapon;

class CAbstractFactory : public CBase
{
	DECLARE_SINGLETON(CAbstractFactory);

private:
	explicit CAbstractFactory();
	virtual ~CAbstractFactory();

public:
	Engine::CGameObject* Create(EObjectType eType) const;

	/* Item */
	Engine::CGameObject* CreateRandomItem(CGameObject* pSpawner) const;

	/* Weapon */
	CWeapon* CraeteWeapon(EObjectType eType) const;

private:
	virtual void Free();

	unordered_map<EObjectType, Engine::CGameObject*(*)(const TCreateDesc&)> m_mapCreator;
};

