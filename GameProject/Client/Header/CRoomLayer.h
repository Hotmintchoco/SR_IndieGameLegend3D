#pragma once

#include "CLayer.h"

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

private:
	int m_iRoomIndex = -1;
	bool m_bCleared = false;

public:
	static CRoomLayer* Create(int iRoomIndex);
};

