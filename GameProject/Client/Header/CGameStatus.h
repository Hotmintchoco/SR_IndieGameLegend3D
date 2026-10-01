#pragma once

#include "CGameObject.h"
#include "Engine_Define.h"

class CRoomLayer;

class CGameStatus : public CGameObject
{

protected:
	explicit CGameStatus(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGameStatus();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	void DebugRayTest();
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

public:
	void RenderImGui();
	void DebugPanelForRendering();

	/* 방 관련 정보 */
	inline void UpdateCurrentRoomIndex(int iIndex) { m_iCurrentRoomIndex = iIndex; }
	inline void UpdateVisitTable(int iIndex) { m_bVisitTable[iIndex] = true; }
	inline void UpdateClearTable(int iIndex) { m_bClearTable[iIndex] = true; }
	inline bool IsVisited(int iIndex) const { return m_bVisitTable[iIndex]; }

	/* 플레이어 관련 정보 */
	inline void UpdatePlayerHp(int iAmount) { m_iPlayerHp += iAmount; }
	inline void SetPlayerHp(int iHp) { m_iPlayerHp = iHp; }
	inline int GetPlayerHp() const { return m_iPlayerHp; }
	inline void UpdatePlayerMaxHp(int iAmount) { m_iPlayerMaxHp += iAmount; }

	/* 무기류 관련 정보 */
	inline void SetUltimateGauge(float fAmount) { m_fUltGauge = fAmount; }
	inline void SetSpecialAttackGauge(float fAmount) { m_fSpecialAtkGauge = fAmount; }
	inline void SetSpecialAttackSwtich(bool bFlag) { m_bSpecialAtkSwitch = bFlag; }
	inline float GetUltimateGauge() const { return m_fUltGauge; }
	inline float GetSpecialAttackGauge() const { return m_fSpecialAtkGauge; }
	inline bool GetSpecialAttackSwitch() const { return m_bSpecialAtkSwitch; }

	/* 자원 관련 정보 */
	inline void UpdateGem(int iAmount) { m_iGem += iAmount; }
	inline int GetGemCount() const { return m_iGem; }

	/* 카메라 관련 정보 */
	inline void UpdateCameraYaw(float fYaw) { m_fYaw = fYaw; }
	inline float GetYaw() const { return m_fYaw; }

	/* 기타 디버깅용 정보 */
	inline void UpdateFPS(float fDT) { m_fDT = fDT; }
	inline void RegisterPseudoDark(CGameObject* pObject) { m_vecPseudoDark.push_back(pObject); }

private:
	void UpdateCameraInfo();

	/* CStage */
	CStage* m_pStage = nullptr;

	/* Minimap */
	int m_iCurrentRoomIndex = -1;
	bool m_bVisitTable[25] = { false };
	bool m_bClearTable[25] = { false };
	_vec3 m_vPlayerPos{0.f, 0.f, 0.f};

	/* Player */
	int m_iPlayerHp = 12;
	int m_iPlayerMaxHp = 12;

	/* Weapon */
	float m_fUltGauge = 0.f;
	float m_fSpecialAtkGauge = 0.f;
	float m_bSpecialAtkSwitch = false;
	
	/* Item */
	int m_iGem = 0;
	
	/* Camera */
	_vec3 m_vCamPos{ 60.f, 0.f, 60.f };
	_vec3 m_vCamLook{ 0.f, 0.f, 1.f };
	float m_fYaw = 0.f;

	/* FPS */
	float m_fDT = 0;

	/* Render Debug */
	bool m_bShowDark = false;
	vector<CGameObject*> m_vecPseudoDark;
	bool m_bDebugTriangle = false;

	/* 사운드 */
	float m_fBGMVolume = 0.5f;
	float m_fSFXVolume = 1.f;
	bool m_bBGMMute = true;
	bool m_bSFXMute = true;

	/* Time Scale */
	float m_fTimeScale = 1.f;
	bool m_bExcludePlayer = false;

public:
	static CGameStatus* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

