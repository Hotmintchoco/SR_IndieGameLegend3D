#pragma once

#include "Engine_Define.h"
#include <vector>
#include <utility>

BEGIN(Engine)

class CLayer;
class CScene;

class ENGINE_DLL CLayerContext
{
public:
	CLayerContext(CLayer* pLayer, CScene* pScene)
	{ 
		m_ContextStack.emplace_back(pLayer, pScene);
	}
	~CLayerContext() 
	{ 
		m_ContextStack.pop_back(); 
	}

	static CLayer* GetLayer()
	{
		if (m_ContextStack.empty())
		{
			return nullptr;
		}
		else
		{
			return m_ContextStack.back().first;
		}
	}

	static CScene* GetScene()
	{
		if (m_ContextStack.empty())
		{
			return nullptr;
		}
		else
		{
			return m_ContextStack.back().second;
		}
	}

private:
	static std::vector<std::pair<CLayer*, CScene*>> m_ContextStack;
};

END

