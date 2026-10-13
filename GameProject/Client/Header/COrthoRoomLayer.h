#pragma once

#include "CRoomLayer.h"

enum GAMEMODE { PERSP, ORTHO };

class COrthoCamera;

class COrthoRoomLayer : public CRoomLayer
{
protected:
	explicit COrthoRoomLayer(int iRoomIndex);
	virtual ~COrthoRoomLayer();

public:
	virtual HRESULT Ready_Layer() override;
	virtual _int Update_Layer(_float fTimeDelta) override;
	virtual void LateUpdate_Layer(_float fTimeDelta) override;

protected:
	virtual HRESULT SpawnRoom() override;

	bool m_bRegistered = false;
	COrthoCamera* m_pOrthoCamera = nullptr;

	GAMEMODE m_eMode = PERSP;

public:
	static COrthoRoomLayer* Create(int iRoomIndex);

protected:
	virtual void Free() override;
};

