#pragma once

#include "Engine_Define.h"

class CGameStatusMgr
{
	DECLARE_SINGLETON(CGameStatusMgr);

private:
	explicit CGameStatusMgr();
	virtual ~CGameStatusMgr();

public:

private:
	/* Debug */
	int m_iCurrentRoomIndex = -1;

	/* UI */
	int m_iPlayerHp = 0;
	int m_iPlayerMaxHp = 0;
	float m_fUltGauge = 0.f;
	float m_fSpecialAtkGauge = 0.f;
	int m_iGem = 0;
	float m_fYaw = 0.f;
	_vec3 m_vPlayerPos = _vec3{ 0.f, 0.f, 0.f };

private:
	virtual void Free();
};

