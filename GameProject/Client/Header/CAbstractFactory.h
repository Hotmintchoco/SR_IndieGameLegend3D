#pragma once

#include "CBase.h"
#include "Client_Struct.h"

namespace Engine
{
	class CGameObject;
	class CScene;
}

struct TCreateDesc
{
	LPDIRECT3DDEVICE9 pDevice = nullptr;
	CGameObject* pSpawner = nullptr;
};

class CWeapon;
class CRoomLayer;

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
	CWeapon* CreateWeapon(EObjectType eType) const;

	/* Room Layer */
	CRoomLayer* CreateRoom(int iType, const int iIndex) const;

	/* MiniGame */
	CScene* CreateScene(ESceneType eType) const;

private:
	virtual void Free();

	unordered_map<EObjectType, Engine::CGameObject*(*)(const TCreateDesc&)> m_mapCreator;
	unordered_map<ERoomType, CRoomLayer*(*)(const int iIndex)> m_mapRoomCreator;
	unordered_map<ESceneType, CScene*(*)()> m_mapSceneCreator;
};

