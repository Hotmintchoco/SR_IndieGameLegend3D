#pragma once

#include "CBase.h"
#include "CTimer.h"
#include "Engine_Define.h"
#include <vector>

BEGIN(Engine)

enum TIME_GROUP { TG_NONE, TG_1, TG_2, TG_3, TG_4, TG_5, TG_6, TG_7, TG_8, TG_9, TG_10, TG_END };

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
	void SetGroupTimeScale(int eGroup, float fScale);
	float GetGroupTimeScale(int eGroup);
	void ClearAllGroupTimeScale();
	void ClearGroupTimeScale(int eGroup);
	float GetGroupTimeDelta(int eGroup);

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