#include "CTimerMgr.h"
#include "CGameObject.h"

IMPLEMENT_SINGLETON(CTimerMgr)


CTimerMgr::CTimerMgr()
{
	ClearAllGroupTimeScale();
}

CTimerMgr::~CTimerMgr()
{
	Free();
}

_float CTimerMgr::Get_TimeDelta(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);

	if (pTimer == nullptr)
		return 0.f;

	return pTimer->Get_TimeDelta();
}

void CTimerMgr::Set_TimeDelta(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);

	if (pTimer == nullptr)
		return;

	pTimer->Update_Timer();
}

void CTimerMgr::ClearGroupTimeScale(TIME_GROUP eGroup)
{
	if (eGroup <= TG_NONE || eGroup >= TG_END) return;
	m_fGroupTimeScale[eGroup] = -1.f;
}

float CTimerMgr::GetGroupTimeDelta(TIME_GROUP eGroup)
{
	return m_fFrameDelta * GetGroupTimeScale(eGroup);
}

void CTimerMgr::SetGroupTimeScale(TIME_GROUP eGroup, float fScale)
{
	if (eGroup <= TG_NONE || eGroup >= TG_END) return;
	m_fGroupTimeScale[eGroup] = fScale;
}

float CTimerMgr::GetGroupTimeScale(TIME_GROUP eGroup)
{
	if (eGroup <= TG_NONE || eGroup >= TG_END) return m_fGlobalTimeScale;

	float fScale = m_fGroupTimeScale[eGroup];
	return (fScale < 0.f) ? m_fGlobalTimeScale : fScale;
}

void CTimerMgr::ClearAllGroupTimeScale()
{
	for (int i = 0; i < TG_END; ++i)
	{
		m_fGroupTimeScale[i] = -1.f;
	}
}

HRESULT CTimerMgr::Ready_Timer(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);

	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_mapTimer.insert({ pTimerTag, pTimer });

	return S_OK;
}

CTimer* CTimerMgr::Find_Timer(const _tchar* pTimerTag)
{
	auto		iter = find_if(m_mapTimer.begin(), m_mapTimer.end(), CTag_Finder(pTimerTag));

	if (iter == m_mapTimer.end())
		return nullptr;
	
	return iter->second;
}

void CTimerMgr::Free()
{
	for_each(m_mapTimer.begin(), m_mapTimer.end(), CDeleteMap());
	m_mapTimer.clear();
}
