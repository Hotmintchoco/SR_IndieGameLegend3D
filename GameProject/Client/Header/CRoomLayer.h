#pragma once

#include "CLayer.h"
#include "CEventDelegate.h"
#include "CClearCondition.h"
#include "Client_Struct.h"

class CTile;

class CRoomLayer : public CLayer
{
private:
	explicit CRoomLayer(int iRoomIndex);
	virtual ~CRoomLayer();

public:
	virtual HRESULT Ready_Layer() override;
	virtual _int Update_Layer(const _float& fTimeDelta) override;
	virtual void LateUpdate_Layer(const _float& fTimeDelta) override;

	HRESULT SpawnRoom();

	inline bool IsCleared() { return m_bCleared; }
	inline bool HasVisited() { return m_bVisited; }
	inline bool IsOnProgress() { return m_bOnProgress; }

	CEventDelegate<TRoomEventCtx> m_OnRoomEvent;
	void OnRoomTriggerBlockCollided();
	void OnButtonInteracted(bool bPressed);

	/* 몬스터 클래스가 사용 */
	inline void IncreaseEntityCount() { ++m_iEntityCount; }
	inline void DecreaseEntityCount() { --m_iEntityCount; }
	inline int GetEntityCount() { return m_iEntityCount; }

	/* 타일이 오염되는 공격 */
	void RequestTileContamination(const _vec3& vPos, int iRange, EContaminateType eType, float fDuration);
	CTile* GetTileFromWorldPosition(const _vec3& vWorldPos);

	/* 방 변경에 따른 조명 조정*/
	void ApplyDarkness();
	void FlickerLight(const float fDuration);

private:
	/* 어둠 스위치 */
	void SetPseudoDark(bool bFlag);
	bool m_bDark = false; // 방의 원래 속성
	bool m_bCurrentDark = false; // Flickering 등으로 인한 현재 방의 불빛 상태
	float m_fLeftFlickerTime = 0.f;
	void FlickerHandling(const Engine::_float& fTimeDelta);

	void CheckClearCondition();
	CTile* GetTileFromIndex2D(const TTileIdx& tIdx);

	/* 오염 타일과 플레이어 */
	void PlayerTileInteraction();

	int m_iRoomIndex = -1;
	_vec3 m_vRoomCenterPos = _vec3{ 0.f, 0.f, 0.f };

	bool m_bCleared = false;
	bool m_bVisited = false;
	bool m_bOnProgress = false;

	vector<CClearCondition*> m_vecClearCondition;
	int m_iEntityCount = 0;

public:
	static CRoomLayer* Create(int iRoomIndex);

private:
	virtual void Free() override;
};

