#pragma once

#include "CRoomLayer.h"

enum GAMEMODE { PERSP, ORTHO };

class COrthoCamera;
class CPlayer;
class CFlatPlayer;
class IVerticalTerrain;

class COrthoRoomLayer : public CRoomLayer
{
protected:
	explicit COrthoRoomLayer(int iRoomIndex);
	virtual ~COrthoRoomLayer();

public:
	virtual HRESULT Ready_Layer() override;
	virtual _int Update_Layer(_float fTimeDelta) override;
	virtual void LateUpdate_Layer(_float fTimeDelta) override;

	const vector<IVerticalTerrain*>& GetVerticalTerrainList() { return m_vecVerticalTerrain; }

protected:
	virtual HRESULT SpawnRoom() override;

	GAMEMODE m_eMode = PERSP;

	bool m_bRegistered = false;
	COrthoCamera* m_pOrthoCamera = nullptr;
	CPlayer* m_pPlayer = nullptr;
	CFlatPlayer* m_pFlatPlayer = nullptr;

	vector<IVerticalTerrain*> m_vecVerticalTerrain;

public:
	static COrthoRoomLayer* Create(int iRoomIndex);

protected:
	virtual void Free() override;
};

