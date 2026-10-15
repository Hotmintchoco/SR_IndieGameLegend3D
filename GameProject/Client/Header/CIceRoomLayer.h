#pragma once

#include "CRoomLayer.h"

class CIceRoomLayer : public CRoomLayer
{
protected:
	explicit CIceRoomLayer(int iRoomIndex);
	virtual ~CIceRoomLayer();

public:
	virtual HRESULT Ready_Layer() override;
	virtual _int Update_Layer(_float fTimeDelta) override;
	virtual void LateUpdate_Layer(_float fTimeDelta) override;

protected:
	virtual HRESULT SpawnRoom() override;


public:
	static CIceRoomLayer* Create(int iRoomIndex);

protected:
	virtual void Free() override;
};

