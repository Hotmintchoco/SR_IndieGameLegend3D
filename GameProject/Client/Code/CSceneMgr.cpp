#include "pch.h"
#include "CSceneMgr.h"
#include "CScene.h"
#include "CManagement.h"

IMPLEMENT_SINGLETON(CSceneMgr);

CSceneMgr::CSceneMgr() : m_pMainScene(nullptr)
{
}

CSceneMgr::~CSceneMgr()
{
	
}

void CSceneMgr::Set_MainScene(CScene* pScene)
{
	if (nullptr == pScene)
		return;

	m_pMainScene = pScene;
}

void CSceneMgr::Change_Scene(CScene* pScene)
{
	if (nullptr == pScene)
		return;
	m_pMainScene = pScene;
}

void CSceneMgr::Free()
{
	if (m_pMainScene)
		Safe_Release(m_pMainScene);
}