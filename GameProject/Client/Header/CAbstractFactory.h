#pragma once

#include "CBase.h"
#include "Client_Struct.h"

namespace Engine
{
	class CGameObject;
}

class CAbstractFactory : public CBase
{
	DECLARE_SINGLETON(CAbstractFactory);

private:
	explicit CAbstractFactory();
	virtual ~CAbstractFactory();

public:
	Engine::CGameObject* Create(EObjectType eType) const;

private:
	virtual void Free();

	unordered_map<EObjectType, Engine::CGameObject*(*)(LPDIRECT3DDEVICE9)> m_mapCreator;
};

