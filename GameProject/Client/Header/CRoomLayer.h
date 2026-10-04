#pragma once

#include "CLayer.h"
#include "CEventDelegate.h"
#include "CClearCondition.h"
#include "Client_Struct.h"

class CTile;
class IRayTestable;
class ITerrain;

class CRoomLayer : public CLayer
{
private:
	explicit CRoomLayer(int iRoomIndex);
	virtual ~CRoomLayer();

public:
	virtual HRESULT Ready_Layer() override;
	virtual _int Update_Layer(_float fTimeDelta) override;
	virtual void LateUpdate_Layer(_float fTimeDelta) override;

	HRESULT SpawnRoom();

	inline int GetIndexFlat() { return m_iRoomIndex; }
	pair<int, int> GetIndex2D();
	inline bool IsCleared() { return m_bCleared; }
	inline bool HasVisited() { return m_bVisited; }
	inline bool IsOnProgress() { return m_bOnProgress; }
	inline _vec3 GetCenterPos() { return m_vRoomCenterPos; }

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

	/* 방 초기화 */
	void ResetState();

	inline bool IsBossRoom() { return m_bBossRoom; }

	/* 레이 테스트용 객체 리스트 */
	vector<IRayTestable*> GetRayTestableList() { return m_vecRayTestable; }
	vector<ITerrain*> GetTerrainList() { return m_vecTerrain; }

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

	/* 인접한 방일때만 업데이트 */
	bool IsValidUpdateTarget();

	int m_iRoomIndex = -1;
	_vec3 m_vRoomCenterPos = _vec3{ 0.f, 0.f, 0.f };

	bool m_bCleared = false;
	bool m_bVisited = false;
	bool m_bOnProgress = false;

	vector<CClearCondition*> m_vecClearCondition;
	int m_iEntityCount = 0;

	/* 바이옴 */
	TBiomeInfo m_tBiomeInfo = {};

	/* 클리어 시 중앙에 리워드 생성 */
	EObjectType m_eClearRewardType = EObjectType::NONE;

	/* 보스 룸 여부 */
	bool m_bBossRoom = false;

	/* 레이 테스트 컨테이너 */
	vector<IRayTestable*> m_vecRayTestable;
	vector<ITerrain*> m_vecTerrain;

public:
	static CRoomLayer* Create(int iRoomIndex);

private:
	virtual void Free() override;
};

