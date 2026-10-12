#pragma once

#include "CRoomLayer.h"

class CAnamorphicObject;
class IAnamorphicTarget;
class CButton;
class CDoor;
class CLiminalFrame;
class CLiminalPortal;
class COpenCube;
class CButtonTile;
class CColorArea;
class CCube;

class CLiminalRoomLayer : public CRoomLayer
{
private:
	explicit CLiminalRoomLayer(int iRoomIndex);
	virtual ~CLiminalRoomLayer();

public:
	virtual HRESULT Ready_Layer() override;
	virtual _int Update_Layer(_float fTimeDelta) override;
	virtual void LateUpdate_Layer(_float fTimeDelta) override;

	virtual HRESULT SpawnRoom() override;


	inline const vector<IAnamorphicTarget*>& GetAnamorphicTargetList() { return m_vecAnamorphicTarget; }

private:
	virtual void OnRoomTriggerBlockCollided() override;
	HRESULT SpawnInnerWalls();
	HRESULT SpawnRoomObjects();
	void OnPhase1Cleared();
	void FlattenWhiteFrame();
	void CheckWhiteFramePickupCondition();

	/* Anamorphic Target Collecting */
	void CollectAnamorphicTargets();
	vector<int> m_vecDisabledTileIdx;

	vector<IAnamorphicTarget*> m_vecAnamorphicTarget;

	bool m_bEventBindSetted = false;

	/* 페이즈 1 */
	CLiminalFrame* m_pFrame_0 = nullptr;
	CLiminalFrame* m_pFrame_1 = nullptr;
	CLiminalFrame* m_pFrame_2 = nullptr;
	CLiminalFrame* m_pFrame_3 = nullptr;
	CLiminalFrame* m_pFrame_4 = nullptr;
	CButton* m_pButtonPhase1 = nullptr;
	COpenCube* m_pOpenCubePhase1 = nullptr;
	vector<CDoor*> m_vecDoorPhase1;
	bool m_bPhase1Cleared = false;

	/* 페이즈 2 */
	CLiminalPortal* m_pPortal_0 = nullptr;
	CLiminalPortal* m_pPortal_1 = nullptr;
	vector<CDoor*> m_vecDoorPhase2;

	/* 페이즈 3 */
	vector<CColorArea*> m_vecColorAreaPhase3;
	vector<CButtonTile*> m_vecButtonTilePhase3;
	CCube* m_pBlockingCube = nullptr;
	int m_iLeftColorCount = 5;
	_vec3 m_vCaptureEye{ 0.f, 0.f, 0.f };
	_vec3 m_vCaptureAt{ 0.f, 0.f, 0.f };
	float m_fAlignedDegreeTH = 1.5f; /* degree */


public:
	static CLiminalRoomLayer* Create(int iRoomIndex);

private:
	virtual void Free() override;
};

