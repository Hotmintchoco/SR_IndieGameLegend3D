#pragma once

#include "CLayer.h"
#include "CEventDelegate.h"

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

	CEventDelegate<void> m_OnRoomBegin;
	void OnRoomBegin();

private:
	int m_iRoomIndex = -1;
	bool m_bCleared = false;
	bool m_bVisited = false;
	bool m_bOnProgress = false;

public:
	static CRoomLayer* Create(int iRoomIndex);
};

