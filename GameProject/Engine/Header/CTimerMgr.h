#pragma once

#include "CBase.h"
#include "CTimer.h"
#include "Engine_Define.h"
#include <vector>

BEGIN(Engine)

enum TIME_GROUP { TG_NONE, TG_PLAYER, TG_END };

class CGameObject;

class ENGINE_DLL CTimerMgr : public CBase
{
	DECLARE_SINGLETON(CTimerMgr)

private:
	explicit CTimerMgr();
	virtual ~CTimerMgr();

public:
	void	Set_FrameDelta(_float fRaw) { m_fFrameDelta = fRaw; }
	_float	Get_FrameDelta() const { return m_fFrameDelta; }

	_float		Get_TimeDelta(const _tchar* pTimerTag);
	void		Set_TimeDelta(const _tchar* pTimerTag);

	/* 시간 스케일 */
	inline void SetGlobalTimeScale(const float fScale) { m_fGlobalTimeScale = fScale; }
	inline float GetGlobalTimeScale() { return m_fGlobalTimeScale; }
	void SetGroupTimeScale(TIME_GROUP eGroup, float fScale);
	float GetGroupTimeScale(TIME_GROUP eGroup);
	void ClearAllGroupTimeScale();
	void ClearGroupTimeScale(TIME_GROUP eGroup);
	float GetGroupTimeDelta(TIME_GROUP eGroup);

public:
	HRESULT		Ready_Timer(const _tchar* pTimerTag);

private:
	CTimer* Find_Timer(const _tchar* pTimerTag);

private:
	map<const _tchar*, CTimer*>			m_mapTimer;

	float m_fFrameDelta = 0.f;
	float m_fGlobalTimeScale = 1.f;
	float m_fGroupTimeScale[TG_END] = { 1.f, 1.f };

private:
	virtual void Free();

};

END