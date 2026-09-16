#pragma once

#include "CLayer.h"
#include "CEventDelegate.h"
#include "CClearCondition.h"
#include "Client_Struct.h"

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

	/* 몬스터 클래스가 사용 */
	inline void IncreaseEntityCount() { ++m_iEntityCount; }
	inline void DecreaseEntityCount() { --m_iEntityCount; }
	inline int GetEntityCount() { return m_iEntityCount; }

private:
	void CheckClearCondition();

	int m_iRoomIndex = -1;
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

