#pragma once

#include "CRoomLayer.h"

class CLaserPuzzleRoomLayer : public CRoomLayer
{
private:
	explicit CLaserPuzzleRoomLayer(int iRoomIndex);
	virtual ~CLaserPuzzleRoomLayer();

public:
	virtual HRESULT Ready_Layer() override;
	virtual _int Update_Layer(_float fTimeDelta) override;
	virtual void LateUpdate_Layer(_float fTimeDelta) override;

private:


public:
	static CLaserPuzzleRoomLayer* Create(int iRoomIndex);

private:
	virtual void Free() override;
};
