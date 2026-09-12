#pragma once

#include "CBase.h"
#include "Define.h"

namespace Engine
{
	class CGameObject;
}

class CAbstractFactory : public CBase
{
public:
	CAbstractFactory();

	Engine::CGameObject* Create(EObjectType eType) const;

private:
	virtual void Free();

	unordered_map<EObjectType, Engine::CGameObject*(*)(LPDIRECT3DDEVICE9)> m_mapCreator;
};

