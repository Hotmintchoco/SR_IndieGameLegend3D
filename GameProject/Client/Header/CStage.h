#pragma once

#include "CScene.h"

namespace Engine
{
	class CGameObject;
}

class CGameStatus;
class CRoomLayer;
class CPlayer;

class CStage : public CScene
{
private:
	explicit CStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage();

public:
	virtual	HRESULT Ready_Scene() override;
	virtual	_int Update_Scene(_float fTimeDelta) override;
	virtual	void LateUpdate_Scene(_float fTimeDelta) override;
	virtual	void Render_Scene() override {}

	virtual HRESULT Add_GameObject(const wstring& pObjTag, CGameObject* pGameObject) override;

	/* 게임 상태관리 오브젝트 직통 부르기 */
	inline CGameStatus* GetStatus() { return m_pStatus; }

	/* 플레이어 상태 관련 */
	void OnPlayerDead();
	void UpdatePlayerPosition(const _vec3& vPos);
	inline _vec3 GetPlayerPosition() { return m_vPlayerPos; }
	inline CPlayer* GetPlayer() { return m_pPlayer; }

	/* 방 관련 */
	inline CRoomLayer* GetCurrentRoomLayer() { return m_pCurrentRoomLayer; }
	inline int GetCurrentRoomIndex() { return m_iCurrentRoomIndex; }
	CRoomLayer* GetRoomLayerFromIndex(int iIndex);

private:
	HRESULT	Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT	Ready_GameLogic_Layer(const _tchar* pLayerTag);
	HRESULT	Ready_Room_Layer(const wstring& wstrLayerTag, int iRoomIdx);
	HRESULT	Ready_UI_Layer(const _tchar* pLayerTag);

	void CheckRoomChanged();
	int CalculateRoomIndexFromPlayerPosition();

	CAMERAID			m_CurCamera; // TODO

	/* 방 관련 */
	CRoomLayer* m_pCurrentRoomLayer = nullptr;
	int m_iStartRoomIndex = 12;
	int m_iCurrentRoomIndex = m_iStartRoomIndex;
	int m_iPrevRoomIndex = m_iStartRoomIndex;
	_vec3 m_vPlayerPos{ 0.f, 0.f, 0.f };
	
	/* 자주 쓰는 게임오브젝트 */
	CGameStatus* m_pStatus = nullptr;
	CPlayer* m_pPlayer = nullptr;

	/* 부활 */
	bool m_bOnPlayerDead = false;
	float m_fReviveTime = 2.f;
	float m_fLeftReviveTime = 0.f;

public:
	static CStage* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;

};

