#pragma once

#include "CRoomLayer.h"

class CLiminalRoomLayer : public CRoomLayer
{
private:
	explicit CLiminalRoomLayer(int iRoomIndex);
	virtual ~CLiminalRoomLayer();

public:
	virtual HRESULT Ready_Layer() override;
	virtual _int Update_Layer(_float fTimeDelta) override;
	virtual void LateUpdate_Layer(_float fTimeDelta) override;

private:


public:
	static CLiminalRoomLayer* Create(int iRoomIndex);

private:
	virtual void Free() override;
};

