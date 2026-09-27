#pragma once
#include "Engine_Define.h"

namespace Engine
{
	class CScene;
}

class CSceneMgr
{
	DECLARE_SINGLETON(CSceneMgr);

private:
	explicit CSceneMgr();
	virtual ~CSceneMgr();

public:
	void		Set_MainScene(CScene* pScene);
	void		Change_Scene(CScene* pScene);

private:
	CScene*		m_pMainScene;

private:
	void		Free();
};

