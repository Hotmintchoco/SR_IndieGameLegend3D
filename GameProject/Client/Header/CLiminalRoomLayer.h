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
	/* 흰 액자 집기 : 정답 시점 근처에서 클릭하면 지금 위치/시선을 저장하고 정답 위치/시선까지 플레이어 카메라를 보간한 뒤 캡처 해제 */
	void StartAlignToCapture();
	void UpdateAlignToCapture(_float fTimeDelta);

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
	float m_fAlignedDegreeTH = 15.f; /* degree (도착 후 보간으로 정확히 맞추므로 진입 판정은 넉넉하게) */

	/* 정답 시점 보간 : 플레이어 위치(눈이 정답 eye에 오도록)와 플레이어 카메라 yaw/pitch를 함께 보간 (무기는 플레이어 카메라를 따라가므로 그대로 붙어 옴) */
	bool m_bFramePickedUp = false;		/* 한 번만 */
	bool m_bAligningToCapture = false;
	/* 클릭한 프레임에는 요청만 : 같은 클릭을 리미널 건이 받아 액자를 집으므로, 버튼을 뗀 뒤(리미널 건이 놓기를 처리한 뒤) 다음 업데이트에서 시작 */
	bool m_bAlignRequested = false;
	float m_fAlignElapsed = 0.f;
	float m_fAlignDuration = 2.f;
	_vec3 m_vAlignStartPos{ 0.f, 0.f, 0.f };
	_vec3 m_vAlignEndPos{ 0.f, 0.f, 0.f };
	float m_fAlignStartYaw = 0.f;
	float m_fAlignStartPitch = 0.f;
	float m_fAlignEndYaw = 0.f;
	float m_fAlignEndPitch = 0.f;


public:
	static CLiminalRoomLayer* Create(int iRoomIndex);

private:
	virtual void Free() override;
};

