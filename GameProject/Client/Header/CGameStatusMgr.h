#pragma once

#include "Engine_Define.h"

class CRoomLayer;

class CGameStatusMgr
{
	DECLARE_SINGLETON(CGameStatusMgr);

private:
	explicit CGameStatusMgr();
	virtual ~CGameStatusMgr();

public:
	void Update(float fTimeDelta);
	void Render();

	void RenderImGui();

	CRoomLayer* GetCurrentRoomLayer();

	inline void UpdatePlayerPosition(const _vec3& vPos) { m_vPlayerPos = vPos; }
	inline void UpdatePlayerHp(int iAmount) { m_iPlayerHp += iAmount; }
	inline void UpdatePlayerMaxHp(int iAmount) { m_iPlayerMaxHp += iAmount; }

	inline void UpdateUltimateGauge(float fAmount) { m_fUltGauge += fAmount; }
	inline void UpdateSpecialAttackGauge(float fAmount) { m_fSpecialAtkGauge += fAmount; }

	inline void UpdateGem(int iAmount) { m_iGem += iAmount; }

	inline void UpdateCameraYaw(float fYaw) { m_fYaw = fYaw; }

	inline void UpdateFPS(float fDT) { m_fDT = fDT; }

private:
	/* Debug */
	int m_iCurrentRoomIndex = -1;

	/* UI */
	_vec3 m_vPlayerPos = _vec3{ 0.f, 0.f, 0.f };
	int m_iPlayerHp = 0;
	int m_iPlayerMaxHp = 0;

	float m_fUltGauge = 0.f;
	float m_fSpecialAtkGauge = 0.f;
	
	int m_iGem = 0;
	
	float m_fYaw = 0.f;

	float m_fDT = 0;

private:
	virtual void Free();
};

