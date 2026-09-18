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
	void Update(const float fTimeDelta);
	void Render();

	void RenderImGui();
	void DebugPanelForRendering();

	CRoomLayer* GetCurrentRoomLayer();

	inline void UpdateCurrentRoomIndex(int iIndex) { m_iCurrentRoomIndex = iIndex; }
	inline void UpdateVisitTable(int iIndex) { m_bVisitTable[iIndex] = true; }
	inline void UpdateClearTable(int iIndex) { m_bClearTable[iIndex] = true; }

	inline void UpdatePlayerPosition(const _vec3& vPos) { m_vPlayerPos = vPos; }
	inline void UpdatePlayerHp(int iAmount) { m_iPlayerHp += iAmount; }
	inline void UpdatePlayerMaxHp(int iAmount) { m_iPlayerMaxHp += iAmount; }

	inline void UpdateUltimateGauge(float fAmount) { m_fUltGauge += fAmount; }
	inline void UpdateSpecialAttackGauge(float fAmount) { m_fSpecialAtkGauge += fAmount; }

	inline void UpdateGem(int iAmount) { m_iGem += iAmount; }

	inline void UpdateCameraYaw(float fYaw) { m_fYaw = fYaw; }

	inline void UpdateFPS(float fDT) { m_fDT = fDT; }

	inline float GetYaw() const { return m_fYaw; }
	inline int GetGemCount() const { return m_iGem; }

private:
	/* Minimap */
	int m_iCurrentRoomIndex = 12;
	bool m_bVisitTable[25] = { false };
	bool m_bClearTable[25] = { false };
	_vec3 m_vPlayerPos = _vec3{ 0.f, 0.f, 0.f };

	/* Player */
	int m_iPlayerHp = 0;
	int m_iPlayerMaxHp = 0;

	/* Weapon */
	float m_fUltGauge = 0.f;
	float m_fSpecialAtkGauge = 0.f;
	
	/* Item */
	int m_iGem = 0;
	
	/* Camera */
	float m_fYaw = 0.f;

	/* FPS */
	float m_fDT = 0;

	/* Render Debug */


private:
	virtual void Free();
};

