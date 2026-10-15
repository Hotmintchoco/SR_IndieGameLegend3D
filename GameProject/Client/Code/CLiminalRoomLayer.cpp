#include "pch.h"
#include "CLiminalRoomLayer.h"
#include "CDInputMgr.h"
#include "CAnamorphicObject.h"
#include "CGraphicDev.h"
#include "CTransform.h"
#include "IAnamorphicTarget.h"
#include "CSoundMgr.h"
#include "CStage.h"
#include "CManagement.h"
#include "CGameStatus.h"
#include "CWall.h"
#include "CBoxCollider.h"
#include "CRoomLoadingMgr.h"
#include "CRandomMgr.h"
#include "Client_Struct.h"
#include "CPlayer.h"
#include "CButton.h"
#include "CFog.h"
#include "CDoor.h"
#include "CLiminalFrame.h"
#include "COpenCube.h"
#include "CLiminalPortal.h"
#include "CCube.h"
#include "CColorArea.h"
#include "CButtonTile.h"
#include "CClientCameraMgr.h"
#include "CCamera.h"

CLiminalRoomLayer::CLiminalRoomLayer(int iRoomIndex)
	: CRoomLayer(iRoomIndex)
{
}

CLiminalRoomLayer::~CLiminalRoomLayer()
{
}

HRESULT CLiminalRoomLayer::Ready_Layer()
{
	return S_OK;
}

_int CLiminalRoomLayer::Update_Layer(_float fTimeDelta)
{
	if (!m_bEventBindSetted)
	{
		/* 이벤트 등록 */
		m_pButtonPhase1->m_OnButtonPressed.AddBinding(GetToken(), [this]() { if (m_bPhase1Cleared) return; OnPhase1Cleared(); });

		/* 문의 경우 맵 전체 이벤트가 아닌 페이즈 단위 이벤트로 처리해야하프로 끊었다 다시 연결 */
		for (auto pDoor : m_vecDoorPhase1)
		{
			int iHandle = pDoor->GetBindHandle();
			m_OnRoomEvent.DeleteBinding(iHandle);
			pDoor->Close();
		}
		for (auto pDoor : m_vecDoorPhase2)
		{
			int iHandle = pDoor->GetBindHandle();
			m_OnRoomEvent.DeleteBinding(iHandle);
			pDoor->Close();
		}

		m_bEventBindSetted = true;

		CManagement::GetInstance()->GetCurrentScene()->Get_Layer(L"GameLogic_Layer")->Add_GameObject(L"Room_18_Frame_4", m_pFrame_4);

		for (auto pArea : m_vecColorAreaPhase3)
		{
			pArea->m_OnColorAreaSatisfied.AddBinding(GetToken(), [this](EColorType eType) { 
				--m_iLeftColorCount; 
				if (m_iLeftColorCount == 1) /* 흰색 제외 다 누름 */
				{
					m_pBlockingCube->Set_Dead(true);
					/* TODO 블럭 깨지는 소리 */
				}
				});
		}
	}

	CheckWhiteFramePickupCondition();

	_int iExit = CRoomLayer::Update_Layer(fTimeDelta);

	return iExit;
}

void CLiminalRoomLayer::LateUpdate_Layer(_float fTimeDelta)
{
	CRoomLayer::LateUpdate_Layer(fTimeDelta);
}

HRESULT CLiminalRoomLayer::SpawnRoom()
{
	CRoomLayer::SpawnRoom();

	if(FAILED(SpawnInnerWalls()))
		return E_FAIL;

	if (FAILED(SpawnRoomObjects()))
		return E_FAIL;

	CollectAnamorphicTargets();

	FlattenWhiteFrame();

	return S_OK;
}

void CLiminalRoomLayer::FlattenWhiteFrame()
{
	// 블럭 (3, 3) 중심
	m_vCaptureEye = GetCenterPos() + _vec3{ -4.f,  1.f, -2.f };
	// 섹션 C 남동쪽 바닥 모서리 (문 오른쪽 아래)
	m_vCaptureAt = GetCenterPos() + _vec3{ -5.5f, 0.f, -0.5f };
	_matrix matWorld(
		0.708803f, 0.00049424f, 0.661619f, 0.f,
		-0.0522727f, 0.966619f, 0.0552785f, 0.f,
		-0.659551f, -0.0760783f, 0.706644f, 0.f,
		GetCenterPos().x - 4.962f, 0.0201977f, GetCenterPos().z - 1.0664f, 1.f
	);
	CTransform* pTransform = dynamic_cast<CTransform*>(m_pFrame_4->Get_Component(ID_DYNAMIC, L"Com_Transform"));
	pTransform->Set_World(&matWorld);
	pTransform->WorldMatrixDecompose();
	m_pFrame_4->ManualCapture(m_vCaptureEye, m_vCaptureAt);
}

void CLiminalRoomLayer::CheckWhiteFramePickupCondition()
{
	if (m_iLeftColorCount > 1) return;

	/* 시각 오차(°) ≈ 위치 오차 / 물체까지 거리 × 57.3 를 참고함 */
	CCamera* pCamera = CClientCameraMgr::GetInstance()->Find_Camera(CLIENT_CAMERA_TYPE::PLAYER);
	_matrix matCamWorld;
	pCamera->GetWorld(&matCamWorld);
	_vec3 vCamLook, vCamPos;
	memcpy(&vCamLook, &matCamWorld.m[2][0], sizeof(_vec3));
	memcpy(&vCamPos, &matCamWorld.m[3][0], sizeof(_vec3));
	D3DXVec3Normalize(&vCamLook, &vCamLook);
	D3DXVec3Normalize(&vCamLook, &vCamLook);

	_vec3 vTowardCapture, vTowardObject;
	vTowardCapture = vCamPos - m_vCaptureEye;
	vTowardObject = m_vCaptureAt - m_vCaptureEye;
	_float fPosErr = D3DXVec3Length(&vTowardCapture);
	_float fObjDist = D3DXVec3Length(&vTowardObject);
	_bool  bAligned = (fPosErr / fObjDist) < D3DXToRadian(m_fAlignedDegreeTH);

	bool bCondition = bAligned && CDInputMgr::GetInstance()->Mouse_Down(DIM_LB);
	if (bCondition)
	{
		/* TODO 카메라 보간 후 캡쳐 */
		m_pFrame_4->ReverseCapture();
	}
}

void CLiminalRoomLayer::OnRoomTriggerBlockCollided()
{
	if (m_bOnProgress) return;
	
	CStage* pStage = static_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());
	if (!pStage) return;

	if (!m_bVisited)
	{
		pStage->GetStatus()->UpdateVisitTable(m_iRoomIndex);
		m_bVisited = true;
	}

	if (!m_bCleared)
	{
		TRoomEventCtx t{ ERoomEventType::ROOM_BEGIN };
		m_OnRoomEvent.Broadcast(t);
		m_bOnProgress = true;
		CSoundMgr::GetInstance()->PlaySFX(L"sfxDoorClose.wav");

		/* 플레이어 크기를 작게 */
		pStage->GetPlayer()->SetPseudoScale(0.1f, 1.f);

		/* 돌리 줌 적용 */
	}
}

/*
	내부 벽 (위에서 본 XZ 평면, 방 중심 기준, 1 = 타일 1칸)
	- 바닥 : X -6.5 ~ 6.5 (13칸), Z -5.5 ~ 5.5 (11칸)
	- 원본 버퍼 위치 : EW 계열은 X +6.5(앞면) ~ +7.5(윗면 끝), NS 계열은 Z +5.5 ~ +6.5
	- WEST/SOUTH는 CWall이 Y축 180도 회전 → 같은 버퍼로 반대쪽을 보는 벽
	- 벽 메시는 한쪽 면(앞면 + 경사 윗면)만 있으므로 등을 맞댄 쌍으로 생성, 두께 2칸

			X -6.5             -0.5   1.5          6.5
	  Z 5.5  +---------------------------------------+
			 |            C : 북쪽 방 13 x 3          |   (북쪽 외곽 문으로 나감)
	  Z 2.5  +===== 가로 서쪽 =====[기둥]=== 가로 동쪽 ===+   (용마루 Z 1.5)
	  Z 0.5  +=====(문 X -4)======[    ]===(문 X 4)====+
			 |  B : 남서 방 6 x 6 |세로|  A : 남동 방 5 x 6 |   (세로 용마루 X 0.5, 문 없음)
	  Z -5.5 +-------------------+----+-----------------+   (동쪽 외곽 문 Z 0 으로 A 진입)

	진행 순서 : A → (동쪽 문, 페이즈 1) → C → (서쪽 문, 페이즈 2) → B

	[가로 서쪽 쌍] Inner_NS_Door6 / Door6_Mirror  앞면 X -6.5 ~ -0.5, 문 X -4.5 ~ -3.5
				  6칸(짝수)이라 문이 반 칸 치우친 비대칭 버퍼 → 180도 회전하는 SOUTH 쪽은 대칭본 사용
				  (두 버퍼를 맞바꾸면 문이 X -3.5 ~ -2.5로 이동)
	[가로 동쪽 쌍] Inner_NS_Door5   앞면 X 1.5 ~ 6.5, 문 X 3.5 ~ 4.5 (대칭 버퍼)
	[세로 쌍]     Inner_EW_NoDoor6 앞면 Z -5.5 ~ 0.5
	[기둥 팁]     Inner_Tip : 기둥(X -0.5 ~ 1.5, Z 0.5 ~ 2.5) 북쪽 앞면 + 윗면 삼각형
*/
HRESULT CLiminalRoomLayer::SpawnInnerWalls()
{
	LPDIRECT3DDEVICE9 pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();
	TRoomData* t = CRoomLoadingMgr::GetInstance()->GetRoomData(m_iRoomIndex);
	const _vec3 vCenter = GetCenterPos();

	/* 텍스처 인덱스 규칙은 CRoomLayer::SpawnRoom과 동일 (EW +100, Door +50, 바이옴 *10, 랜덤 0~9) */
	auto MakeTextureIdx = [&](bool bEW, bool bDoorTexture)
		{
			return (bEW ? 100 : 0) + (bDoorTexture ? 50 : 0) + t->iBiome * 10
				+ CRandomMgr::GetInstance()->GetRandomValue<int>(0, 9);
		};

	/* 박스 콜라이더 하나 설정값 (DiffPos는 벽 위치 기준, 회전 영향 없음) */
	struct TBox { _vec3 vExtents; _vec3 vDiff; };

	/* 벽 하나 생성 + 버퍼 교체 + 배치 + 콜라이더 2개 설정 */
	auto SpawnWall = [&](EWallDir eDir, int iTextureIdx, const _tchar* pBufferProtoTag,
		const wstring& wstrSuffix, _float fOffsetX, _float fOffsetZ, const TBox& tBox0, const TBox& tBox1) -> HRESULT
		{
			/* bHasDoor = false : 방 이벤트(BlockDoor)가 콜라이더 설정을 덮어쓰지 않도록.
			   문 텍스처는 텍스처 인덱스(+50)로 지정 */
			CWall* pWall = CWall::Create(pDevice, eDir, false, iTextureIdx);
			if (nullptr == pWall)
				return E_FAIL;

			if (FAILED(pWall->ReplaceBuffer(pBufferProtoTag)))
			{
				Safe_Release(pWall);
				return E_FAIL;
			}

			wstring wstrName = L"Room_" + to_wstring(m_iRoomIndex) + L"_InnerWall_" + wstrSuffix;
			if (FAILED(Add_GameObject(wstrName, pWall)))
				return E_FAIL;

			pWall->GetTransform()->Set_Pos(vCenter.x + fOffsetX, 0.f, vCenter.z + fOffsetZ);

			const TBox* pBoxes[2] = { &tBox0, &tBox1 };
			const _tchar* pTags[2] = { L"Com_Collider0", L"Com_Collider1" };
			for (int i = 0; i < 2; ++i)
			{
				CBoxCollider* pCollider = static_cast<CBoxCollider*>(pWall->Get_Component(ID_DYNAMIC, pTags[i]));
				if (nullptr == pCollider)
					return E_FAIL;
				pCollider->Set_Extents(pBoxes[i]->vExtents.x, pBoxes[i]->vExtents.y, pBoxes[i]->vExtents.z);
				pCollider->Set_DiffPos(pBoxes[i]->vDiff);
			}

			m_vecRayTestable.push_back(pWall);
			return S_OK;
		};

	/* 문 1개 + 안개 5장 (CRoomLayer::SpawnRoom 외벽 로직과 동일한 수치)
	   외벽 공식 : 기준점(방 중심) + vDir * 거리. 기준점 ~ 앞면 거리 = 5.5 (NS) / 6.5 (EW)
	   내벽은 기준점 = 벽 위치(벽 길이 방향 좌표만 문 중심으로) 로 바꿔서 그대로 적용
	   iDir : 1 동(+X) 2 남(-Z) 3 서(-X) 4 북(+Z) = 앞면에서 통로가 이어지는 방향
	   만든 문은 vecDoorOut에 담아 페이즈 이벤트에서 여닫을 수 있게 함 */
	auto SpawnDoorAndFog = [&](const wstring& wstrSuffix, _float fBaseX, _float fBaseZ, int iDir,
		vector<CDoor*>& vecDoorOut) -> HRESULT
		{
			const _vec3 vBase{ vCenter.x + fBaseX, 0.f, vCenter.z + fBaseZ };

			_vec3 vDir{ 0.f, 0.f, 1.f };
			_matrix matRot;
			D3DXMatrixRotationY(&matRot, D3DXToRadian(90.f) * iDir);
			D3DXVec3TransformCoord(&vDir, &vDir, &matRot);

			const wstring wstrPrefix = L"Room_" + to_wstring(m_iRoomIndex) + L"_InnerDoor_" + wstrSuffix;

			/* 안개 */
			for (int i = 0; i < 5; ++i)
			{
				CGameObject* pFog = CFog::Create(pDevice);
				if (nullptr == pFog)
					return E_FAIL;

				wstring wstrFogName = wstrPrefix + L"_Fog_" + to_wstring(i);
				if (FAILED(Add_GameObject(wstrFogName, pFog)))
					return E_FAIL;

				static_cast<CFog*>(pFog)->SetOpacity(i == 4 ? 100 : 40);

				CTransform* pTransformCom = dynamic_cast<CTransform*>(pFog->Get_Component(ID_DYNAMIC, L"Com_Transform"));
				pTransformCom->Set_Scale(_vec3{ 0.5f, 0.75f, 1.f });
				pTransformCom->Set_Pos(vBase.x, 0.75f, vBase.z);
				pTransformCom->Rotation(ROT_Y, 90.f * iDir);
				pTransformCom->Move_Pos(&vDir, 5.7f + (iDir % 2) * 1.f + 0.22f * i, 1.f);
			}

			/* 문 */
			CDoor* pDoor = static_cast<CDoor*>(CDoor::Create(pDevice));
			if (nullptr == pDoor)
				return E_FAIL;

			if (FAILED(Add_GameObject(wstrPrefix, pDoor)))
				return E_FAIL;

			CTransform* pTransformCom = dynamic_cast<CTransform*>(pDoor->Get_Component(ID_DYNAMIC, L"Com_Transform"));
			pTransformCom->Set_Pos(vBase.x, 0.75f, vBase.z);
			pTransformCom->Rotation(ROT_Y, 90.f * iDir);
			pTransformCom->Move_Pos(&vDir, 5.5f + (iDir % 2) * 1.f, 1.f);

			vecDoorOut.push_back(pDoor);
			return S_OK;
		};

	/* 쌍 공통 : A(NORTH/EAST)는 벽 위치 기준 +6/+7, B(SOUTH/WEST)는 -6/-7 → 각 벽이 자기 두께 1칸만 담당 */

	/* 1. 가로 서쪽 쌍 : NS Door6 / Door6_Mirror, 위치 X -3.5. 문 X -4.5 ~ -3.5 (C ↔ B, 페이즈 2)
		  콜라이더 : 서쪽 2칸(X -6.5 ~ -4.5) → 벽 기준 DiffX -2, 반폭 1
					동쪽 3칸(X -3.5 ~ -0.5) → 벽 기준 DiffX +1.5, 반폭 1.5 */
	{
		const int iTex = MakeTextureIdx(false, true);

		if (FAILED(SpawnWall(EWallDir::NORTH, iTex, L"Proto_Wall_Inner_NS_Door6_Buffer", L"NS_W_0", -3.5f, -5.f,
			{ { 1.f, 4.f, 0.5f }, { -2.f, 2.f,  6.f } }, { { 1.5f, 4.f, 0.5f }, { 1.5f, 2.f,  6.f } })))
			return E_FAIL;
		if (FAILED(SpawnWall(EWallDir::SOUTH, iTex, L"Proto_Wall_Inner_NS_Door6_Mirror_Buffer", L"NS_W_1", -3.5f, 8.f,
			{ { 1.f, 4.f, 0.5f }, { -2.f, 2.f, -6.f } }, { { 1.5f, 4.f, 0.5f }, { 1.5f, 2.f, -6.f } })))
			return E_FAIL;

		/* 문 + 안개 : B 쪽(앞면 Z 0.5, 통로 +Z) / C 쪽(앞면 Z 2.5, 통로 -Z), 문 중심 X -4 */
		if (FAILED(SpawnDoorAndFog(L"NS_W_0", -4.f, -5.f, 4, m_vecDoorPhase2)))
			return E_FAIL;
		if (FAILED(SpawnDoorAndFog(L"NS_W_1", -4.f, 8.f, 2, m_vecDoorPhase2)))
			return E_FAIL;
	}

	/* 2. 가로 동쪽 쌍 : NS Door5, 위치 X 4. 문 X 3.5 ~ 4.5 (A ↔ C, 페이즈 1)
		  콜라이더 : 문 양옆 2칸씩 → 벽 기준 DiffX ±1.5, 반폭 1 */
	{
		const int iTex = MakeTextureIdx(false, true);
		const _tchar* pTag = L"Proto_Wall_Inner_NS_Door5_Buffer";

		if (FAILED(SpawnWall(EWallDir::NORTH, iTex, pTag, L"NS_E_0", 4.f, -5.f,
			{ { 1.f, 4.f, 0.5f }, { -1.5f, 2.f,  6.f } }, { { 1.f, 4.f, 0.5f }, { 1.5f, 2.f,  6.f } })))
			return E_FAIL;
		if (FAILED(SpawnWall(EWallDir::SOUTH, iTex, pTag, L"NS_E_1", 4.f, 8.f,
			{ { 1.f, 4.f, 0.5f }, { -1.5f, 2.f, -6.f } }, { { 1.f, 4.f, 0.5f }, { 1.5f, 2.f, -6.f } })))
			return E_FAIL;

		/* 문 + 안개 : A 쪽(앞면 Z 0.5, 통로 +Z) / C 쪽(앞면 Z 2.5, 통로 -Z), 문 중심 X 4 */
		if (FAILED(SpawnDoorAndFog(L"NS_E_0", 4.f, -5.f, 4, m_vecDoorPhase1)))
			return E_FAIL;
		if (FAILED(SpawnDoorAndFog(L"NS_E_1", 4.f, 8.f, 2, m_vecDoorPhase1)))
			return E_FAIL;
	}

	/* 3. 세로 쌍 : EW NoDoor6, 위치 X -7 / 8, Z -2.5 → 앞면 X -0.5 / 1.5, 용마루 X 0.5
		  콜라이더 : 6칸을 3칸씩 두 박스로 (기본값은 Z ±7까지 뻗어 C를 막으므로 덮어씀) */
	{
		const int iTex = MakeTextureIdx(true, false);
		const _tchar* pTag = L"Proto_Wall_Inner_EW_NoDoor6_Buffer";

		if (FAILED(SpawnWall(EWallDir::EAST, iTex, pTag, L"EW_0", -7.f, -2.5f,
			{ { 0.5f, 4.f, 1.5f }, {  7.f, 2.f, -1.5f } }, { { 0.5f, 4.f, 1.5f }, {  7.f, 2.f, 1.5f } })))
			return E_FAIL;
		if (FAILED(SpawnWall(EWallDir::WEST, iTex, pTag, L"EW_1", 8.f, -2.5f,
			{ { 0.5f, 4.f, 1.5f }, { -7.f, 2.f, -1.5f } }, { { 0.5f, 4.f, 1.5f }, { -7.f, 2.f, 1.5f } })))
			return E_FAIL;
	}

	/* 4. 기둥 팁 : SOUTH(북쪽을 봄), 위치 (X 0.5, Z 8) → 앞면 Z 2.5, 꼭짓점 (X 0.5, Z 1.5)
		  콜라이더 : 기둥 2x2 (X -0.5 ~ 1.5, Z 0.5 ~ 2.5) 중심 = 벽 기준 (0, -6.5). 두 박스 동일 */
	{
		const TBox tPillar{ { 1.f, 4.f, 1.f }, { 0.f, 2.f, -6.5f } };

		if (FAILED(SpawnWall(EWallDir::SOUTH, MakeTextureIdx(false, false), L"Proto_Wall_Inner_Tip_Buffer",
			L"Tip", 0.5f, 8.f, tPillar, tPillar)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLiminalRoomLayer::SpawnRoomObjects()
{
	LPDIRECT3DDEVICE9 pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();
	const _vec3 vCenter = GetCenterPos();

	/* 페이즈 1 */
	{
		/* 오픈 큐브 + 버튼 한 쌍 : 구역 A(X 1.5 ~ 6.5, Z -5.5 ~ 0.5) 가운데
		   - 큐브는 한 변 1 (로컬 -0.5 ~ 0.5) 고정이라 스케일 설정 없음
		   - 큐브 중심 높이 fCubeY → 큐브 범위 Y fCubeY - 0.5 ~ fCubeY + 0.5
		   - 버튼은 큐브 윗면 높이(fCubeY + 0.5)에 올림 */
		const _float fAreaX = 4.f;      /* 구역 A 가운데 X */
		const _float fAreaZ = -2.5f;    /* 구역 A 가운데 Z */
		const _float fCubeY = 1.5f;     /* 큐브 중심 높이 */
		const _float fButtonY = fCubeY + 0.5f;

		COpenCube* pCube = COpenCube::Create(pDevice, L"Proto_Tile_Texture");
		if (nullptr == pCube)
			return E_FAIL;

		wstring wstrCubeName = L"Room_" + to_wstring(m_iRoomIndex) + L"_OpenCubePhase1";
		if (FAILED(Add_GameObject(wstrCubeName, pCube)))
			return E_FAIL;

		pCube->GetTransform()->Set_Pos(vCenter.x + fAreaX, fCubeY, vCenter.z + fAreaZ);
		pCube->SetTextureIndex(47);

		/* 넣으니까 더 어색해짐 */
		//if (IRayTestable* pObj = dynamic_cast<IRayTestable*>(pCube))
		//{
		//	m_vecRayTestable.push_back(pObj);
		//}

		if (ITerrain* pObj = dynamic_cast<ITerrain*>(pCube))
		{
			m_vecTerrain.push_back(pObj);
		}

		m_pOpenCubePhase1 = pCube;

		CButton* pButton = CButton::Create(pDevice);
		if (nullptr == pButton)
			return E_FAIL;

		wstring wstrName = L"Room_" + to_wstring(m_iRoomIndex) + L"_ButtonPhase1";
		if (FAILED(Add_GameObject(wstrName, pButton)))
			return E_FAIL;

		CTransform* pTransformCom = dynamic_cast<CTransform*>(pButton->Get_Component(ID_DYNAMIC, L"Com_Transform"));
		pTransformCom->Set_Pos(vCenter.x + fAreaX, fButtonY, vCenter.z + fAreaZ);

		if (IRayTestable* pObj = dynamic_cast<IRayTestable*>(pButton))
		{
			m_vecRayTestable.push_back(pObj);
		}

		m_pButtonPhase1 = pButton;

	}

	/* 액자 : 동쪽 문으로 들어와 서쪽을 보면 왼쪽 = 남쪽 */
	{
		/* 0 ~ 3번 : 구역 A의 남쪽 외곽벽(앞면 Z -5.5, X 1.5 ~ 6.5 = 5칸)에 4개, 벽 가운데(X 4) 기준 좌우 대칭, 문에 가까운 쪽부터 0번
		   → X 5.5 / 4.5 / 3.5 / 2.5
		4번(흰색) : 페이즈 3 방으로 옮김. 여기서는 생성/버퍼/텍스처만 하고 위치는 FlattenWhiteFrame()에서 월드 행렬로 지정
		- 액자 메시 원점 = 밑바닥 중앙
		- 북쪽(+Z, 방 안쪽)을 보도록 Y축 180도 회전 (메시 정면이 -Z 기준. 뒤집혀 보이면 회전 제거)
		- 벽에서 fWallGap 만큼 띄워 Z-fighting 방지, 밑바닥 높이 fHangY */
		const _float fWallZ = -5.5f;   /* 남쪽 외곽벽 앞면 */
		const _float fWallGap = 0.02f;   /* 벽에서 띄우는 거리 */
		const _float fHangY = 0.5f;    /* 액자 밑바닥 높이 */
		const _float fScale = 0.7f;    /* 초기 스케일 */
		const _float fStartX = 5.5f;    /* 0번 액자 X (동쪽 문 쪽) */
		const _float fSpacing = 1.f;     /* 액자 간격 (서쪽으로) */

		CLiminalFrame** ppFrames[5] = { &m_pFrame_0, &m_pFrame_1, &m_pFrame_2, &m_pFrame_3, &m_pFrame_4 };
		EColorType eTypes[4] = { EColorType::RED, EColorType::GREEN, EColorType::BLUE, EColorType::YELLOW };

		for (int i = 0; i < 5; ++i)
		{
			CLiminalFrame* pFrame = CLiminalFrame::Create(pDevice);
			if (nullptr == pFrame)
				return E_FAIL;

			wstring wstrFrameName = L"Room_" + to_wstring(m_iRoomIndex) + L"_Frame_" + to_wstring(i);
			if (i == 4)
			{
			}
			else
			{
				if (FAILED(Add_GameObject(wstrFrameName, pFrame)))
					return E_FAIL;
				pFrame->SetFrameColor(eTypes[i]);
				pFrame->SetOutlineColor(eTypes[i]);
			}

			/* 벽에 거는 건 0 ~ 3번만 (4번은 FlattenWhiteFrame에서 배치) */
			if (i < 4)
			{
				CTransform* pFrameTransform = dynamic_cast<CTransform*>(pFrame->Get_Component(ID_DYNAMIC, L"Com_Transform"));
				pFrameTransform->Set_Scale(_vec3{ fScale, fScale, fScale });
				pFrameTransform->Rotation(ROT_Y, 180.f);
				pFrameTransform->Set_Pos(vCenter.x + fStartX - fSpacing * (_float)i, fHangY, vCenter.z + fWallZ + fWallGap);
			}

			pFrame->SetBuffer(L"Proto_Frame_" + to_wstring(i) + L"_Buffer");
			pFrame->SetTexture(L"Proto_Frame_" + to_wstring(i) + L"_Texture");

			m_vecRayTestable.push_back(pFrame);
			m_vecTerrain.push_back(pFrame);

			*ppFrames[i] = pFrame;
		}
	}

	/* 페이즈 2 */
	{
		/* 포탈 2개 : 구역 C(북쪽 방, X -6.5 ~ 6.5, Z 2.5 ~ 5.5) 서쪽/동쪽 끝 가운데에서 서로 마주봄
		   - 가로 내벽 문(X 4 / X -4), 북쪽 외곽 문(X 0)과 겹치지 않음
		   - C는 깊이 3칸인데 포탈 폭이 4라서, 양 기둥이 벽(Z 2.5 / 5.5) 안으로 0.5씩 묻힘 (포탈면 Z 3 ~ 5 는 온전히 보임)
		   - 포탈 메시 : 원점 = 바닥 중앙, 폭 X -2 ~ 2 (기둥 포함), 두께 Z -0.5 ~ 0.5, 정면 -Z 기준
		   - 0번 : 서쪽 외곽벽 쪽, 동쪽(+X)을 봄 → Y축 -90도
			 1번 : 동쪽 외곽벽 쪽, 서쪽(-X)을 봄 → Y축 +90도
			 (메시 정면이 +Z라 서로 등지게 보이면 두 각도의 부호를 바꾸면 됨) */
		const _float fPortalScale = 0.3f;
		const _float fPortalX[2] = { 5.5f, 5.5f };
		const _float fPortalZ[2] = { 3.25f, 4.75f };   /* 동쪽 끝에서 Z 방향으로 나란히 */
		const _float fYaw[2] = { -90.f, -90.f };   /* 둘 다 기존 동쪽 포탈 방향 */
		CLiminalPortal** ppPortals[2] = { &m_pPortal_0, &m_pPortal_1 };

		for (int i = 0; i < 2; ++i)
		{
			CLiminalPortal* pPortal = CLiminalPortal::Create(pDevice);
			if (nullptr == pPortal)
				return E_FAIL;

			wstring wstrPortalName = L"Room_" + to_wstring(m_iRoomIndex) + L"_Portal_" + to_wstring(i);
			if (FAILED(Add_GameObject(wstrPortalName, pPortal)))
				return E_FAIL;

			pPortal->GetTransform()->Set_Scale(_vec3{ fPortalScale, fPortalScale, fPortalScale });
			pPortal->GetTransform()->Rotation(ROT_Y, fYaw[i]);
			pPortal->GetTransform()->Set_Pos(vCenter.x + fPortalX[i], 0.f, vCenter.z + fPortalZ[i]);
			
			// /* 포탈 콜라이더의 DiffPos/Extents는 회전이 반영되지 않으므로 ±90도에 맞춰 직접 회전
			//    (CLiminalPortal.cpp의 FRAME_PILLAR_* / ANIM_* 값 기준)
			//    Y축 θ 회전 : (x, y, z) → (x cosθ + z sinθ, y, -x sinθ + z cosθ), ±90도면 X/Z 반폭 교환 */
			// const _float fSin = (fYaw[i] > 0.f) ? 1.f : -1.f;
			// auto RotateOffset = [fSin](const _vec3& v) { return _vec3{ v.z * fSin, v.y, -v.x * fSin }; };
			// 
			// CBoxCollider* pPillarL = static_cast<CBoxCollider*>(pPortal->Get_Component(ID_DYNAMIC, L"Com_FrameColliderL"));
			// CBoxCollider* pPillarR = static_cast<CBoxCollider*>(pPortal->Get_Component(ID_DYNAMIC, L"Com_FrameColliderR"));
			// CBoxCollider* pAnim = static_cast<CBoxCollider*>(pPortal->Get_Component(ID_DYNAMIC, L"Com_AnimCollider"));
			// if (nullptr == pPillarL || nullptr == pPillarR || nullptr == pAnim)
			// 	return E_FAIL;
			// 
			// pPillarL->Set_Extents(_vec3{ 0.5f, 2.f, 0.5f });
			// pPillarL->Set_DiffPos(RotateOffset(_vec3{ -1.5f, 2.f, 0.f }));
			// pPillarR->Set_Extents(_vec3{ 0.5f, 2.f, 0.5f });
			// pPillarR->Set_DiffPos(RotateOffset(_vec3{ 1.5f, 2.f, 0.f }));
			// pAnim->Set_Extents(_vec3{ 0.125f, 1.5f, 1.f });   /* (1, 1.5, 0.125)의 X/Z 교환 */
			// pAnim->Set_DiffPos(RotateOffset(_vec3{ 0.f, 1.5f, 0.f }));

			if (IRayTestable* pObj = dynamic_cast<IRayTestable*>(pPortal))
			{
				m_vecRayTestable.push_back(pObj);
			}

			*ppPortals[i] = pPortal;
		}

		/* 서로 연결 */
		m_pPortal_0->SetLinkedPortal(m_pPortal_1);
		m_pPortal_1->SetLinkedPortal(m_pPortal_0);

		/* 큐브 8개 : 긴 구역(북쪽 방) 가운데에 세워서 쌓은 3x3 고리 (가운데 한 칸 비움)
			ㅇㅇㅇ   Y 2.5
			ㅇ  ㅇ   Y 1.5   ← 빈 칸 (높이 1 ~ 2)
			ㅇㅇㅇ   Y 0.5
		- X 0 에서 Z 3 ~ 5 (방 깊이 3칸 전체)에 YZ 평면으로 세움
			→ 양 끝 포탈(X ±5.5)이 이 고리의 빈 칸을 사이에 두고 마주봄 (포탈면 높이 0 ~ 3, 중심 1.5와 일치)
		- 큐브는 한 변 1, 원점 = 중심 */
		{
			const _float fFrameX = 0.f;     /* 고리 X (방 가운데) */
			const _float fFrameZ = 4.f;     /* 고리 중심 Z (구역 가운데) */
			const int iCubeTextureIdx = 47;  /* Proto_Tile_Texture 인덱스 */

			int iCubeCount = 0;
			for (int iRow = 0; iRow < 3; ++iRow)        /* 높이 방향 (아래 → 위) */
			{
				for (int iCol = 0; iCol < 3; ++iCol)    /* Z 방향 (남 → 북) */
				{
					if (iRow == 1 && iCol == 1)
						continue;   /* 가운데 빈 칸 */

					CCube* pCube = CCube::Create(pDevice, L"Proto_Tile_Texture");
					if (nullptr == pCube)
						return E_FAIL;

					wstring wstrCubeName = L"Room_" + to_wstring(m_iRoomIndex) + L"_CubePhase2_" + to_wstring(iCubeCount++);
					if (FAILED(Add_GameObject(wstrCubeName, pCube)))
						return E_FAIL;

					pCube->SetTextureIndex(iCubeTextureIdx);
					pCube->GetTransform()->Set_Pos(
						vCenter.x + fFrameX,
						0.5f + (_float)iRow,
						vCenter.z + fFrameZ + (_float)(iCol - 1));

					m_vecRayTestable.push_back(pCube);
					m_vecTerrain.push_back(pCube);
				}
			}
		}
	}

	
	/* 페이즈 3 */
	{
		/* 구역 남서 방(X -6.5 ~ -0.5, Z -5.5 ~ 0.5)에 색깔 영역 4개 + 고정(fixed) 버튼 타일 4개
		- 색깔 영역 : 한 변 2 (CColorArea가 스케일 2로 설정), 중심 = 타일 격자 교차점, 밑면이 바닥에 닿도록 Y 1
		- 버튼 타일 : 각 색깔 영역 안의 타일 하나. 그 자리의 기존 바닥 타일은 비활성화해서 렌더가 겹치지 않게 함
		- 타일 인덱스 규칙은 CRoomLayer::SpawnRoom과 동일 : iTileX = X + (가로 - 1) / 2, iTileZ = (세로 - 1) / 2 - Z, 인덱스 = iTileZ * 가로 + iTileX

		색    버튼 타일 (X, Z)   색깔 영역 중심 (X, Z)
		R     (-1,  0)          (-1.5, -0.5)
		G     (-1, -2)          (-1.5, -2.5)
		Y     (-6, -5)          (-5.5, -4.5)
		B     (-4, -5)          (-3.5, -4.5) */
		struct TColorSpot { EColorType eColor; int iButtonX; int iButtonZ; _float fAreaX; _float fAreaZ; };
		const TColorSpot tSpots[5] =
		{
			{ EColorType::RED,    -1,  0, -1.5f, -0.5f },
			{ EColorType::GREEN,  -1, -2, -1.5f, -2.5f },
			{ EColorType::YELLOW, -6, -5, -5.5f, -4.5f },
			{ EColorType::BLUE,   -4, -5, -3.5f, -4.5f },
			{ EColorType::WHITE,  -1, -5, -1.5f, -4.5f },
		};

		const _vec3 vInnerRoomSize = CRoomLoadingMgr::GetInstance()->GetInnerRoomSize();
		const int iSizeX = (int)vInnerRoomSize.x;
		const int iSizeZ = (int)vInnerRoomSize.z;

		for (int i = 0; i < 5; ++i)
		{
			const TColorSpot& tSpot = tSpots[i];

			/* 색깔 영역 */
			CColorArea* pArea = CColorArea::Create(pDevice);
			if (nullptr == pArea)
				return E_FAIL;

			wstring wstrAreaName = L"Room_" + to_wstring(m_iRoomIndex) + L"_ColorAreaPhase3_" + to_wstring(i);
			if (FAILED(Add_GameObject(wstrAreaName, pArea)))
				return E_FAIL;

			pArea->SetColor(tSpot.eColor);
			CTransform* pAreaTransform = dynamic_cast<CTransform*>(pArea->Get_Component(ID_DYNAMIC, L"Com_Transform"));
			pAreaTransform->Set_Pos(vCenter.x + tSpot.fAreaX, 1.f, vCenter.z + tSpot.fAreaZ);

			m_vecColorAreaPhase3.push_back(pArea);

			/* 버튼 타일 자리의 기존 바닥 타일 비활성화 */
			const int iTileX = tSpot.iButtonX + (iSizeX - 1) / 2;
			const int iTileZ = (iSizeZ - 1) / 2 - tSpot.iButtonZ;
			const int iTileIdx = iTileZ * iSizeX + iTileX;

			wstring wstrTileName = L"Room_" + to_wstring(m_iRoomIndex) + L"_Tile_" + to_wstring(iTileIdx);
			if (CGameObject* pOldTile = Get_GameObject(wstrTileName))
			{
				pOldTile->Set_IsActive(false);
			}

			/* 버튼 타일 (fixed) */
			CButtonTile* pButtonTile = CButtonTile::Create(pDevice, iTileIdx, true);
			if (nullptr == pButtonTile)
				return E_FAIL;

			wstring wstrButtonName = L"Room_" + to_wstring(m_iRoomIndex) + L"_ButtonTilePhase3_" + to_wstring(i);
			if (FAILED(Add_GameObject(wstrButtonName, pButtonTile)))
				return E_FAIL;

			CTransform* pButtonTransform = dynamic_cast<CTransform*>(pButtonTile->Get_Component(ID_DYNAMIC, L"Com_Transform"));
			pButtonTransform->Set_Pos(vCenter.x + (_float)tSpot.iButtonX, 0.f, vCenter.z + (_float)tSpot.iButtonZ);

			m_vecButtonTilePhase3.push_back(pButtonTile);
			m_vecDisabledTileIdx.push_back(iTileIdx);
		}

		/* 막는 큐브 : 섹션 C 블럭 인덱스 (3, 3)
		- 섹션 C 좌상단(북서) 칸 = (X -6, Z 0), 행은 남쪽(-Z), 열은 동쪽(+X)으로 증가
		- (3, 3) → X -6 + 3 = -3, Z 0 - 3 = -3
		- 큐브 원점 = 중심, 한 변 1 → Y 0.5 면 바닥 위에 한 칸 */
		{
			const int iBlockRow = 2;
			const int iBlockCol = 2		;
			const _float fBlockX = -6.f + (_float)iBlockCol;
			const _float fBlockZ = 0.f - (_float)iBlockRow;

			CCube* pCube = CCube::Create(pDevice, L"Proto_Tile_Texture");
			if (nullptr == pCube)
				return E_FAIL;

			wstring wstrCubeName = L"Room_" + to_wstring(m_iRoomIndex) + L"_BlockingCube";
			if (FAILED(Add_GameObject(wstrCubeName, pCube)))
				return E_FAIL;

			pCube->SetTextureIndex(47);
			pCube->GetTransform()->Set_Pos(vCenter.x + fBlockX, 0.5f, vCenter.z + fBlockZ);

			m_pBlockingCube = pCube;
		}
	}

	return S_OK;
}

void CLiminalRoomLayer::OnPhase1Cleared()
{
	/* 문 열기 */
	for (auto pDoor : m_vecDoorPhase1)
	{
		pDoor->Open();
		m_bPhase1Cleared = true;
	}
	for (auto pDoor : m_vecDoorPhase2)
	{
		pDoor->Open();
		m_bPhase1Cleared = true;
	}
	CSoundMgr::GetInstance()->PlaySFX(L"sfxDoorOpen.wav");
}

/*
	아나모픽 대상 수집 : 페이즈 3 구역(남서 방, X -6.5 ~ -0.5, Z -5.5 ~ 0.5)을 둘러싼 것만
	1. 외곽 벽 : 남쪽, 서쪽  (CRoomLayer::SpawnRoom은 동남서북 순으로 Room_N_Wall_0 ~ 3 생성 → 남 1, 서 2)
	2. 내부 벽 : 이 구역을 바라보는 면만
				 - NS_W_0 : 가로 서쪽 쌍 중 NORTH(앞면 Z 0.5, 남쪽 = 이 구역을 봄)
				 - EW_0   : 세로 쌍 중 EAST(앞면 X -0.5, 서쪽 = 이 구역을 봄)
	3. 바닥 타일 : 이 구역 타일 (타일 중심 X -6 ~ -1, Z -5 ~ 0), 버튼 타일 자리로 비활성화한 타일은 제외
	4. 버튼 타일 : 페이즈 3 버튼 타일 4개
*/
void CLiminalRoomLayer::CollectAnamorphicTargets()
{
	m_vecAnamorphicTarget.clear();

	const wstring wstrRoom = L"Room_" + to_wstring(m_iRoomIndex);

	auto AddByName = [&](const wstring& wstrName)
		{
			if (IAnamorphicTarget* pTarget = dynamic_cast<IAnamorphicTarget*>(Get_GameObject(wstrName)))
			{
				m_vecAnamorphicTarget.push_back(pTarget);
			}
		};

	/* 1. 외곽 벽 (남, 서) */
	AddByName(wstrRoom + L"_Wall_1");
	AddByName(wstrRoom + L"_Wall_2");

	/* 1-1. 서쪽 외곽 벽의 문 (CRoomLayer::SpawnRoom 이름 규칙 : Room_N_Door_{벽 인덱스}, 서 = 2) */
	AddByName(wstrRoom + L"_Door_2");

	/* 2. 내부 벽 (이 구역을 바라보는 면) */
	AddByName(wstrRoom + L"_InnerWall_NS_W_0");
	AddByName(wstrRoom + L"_InnerWall_EW_0");

	/* 3. 바닥 타일 (비활성화된 타일 제외) */
	const _vec3 vInnerRoomSize = CRoomLoadingMgr::GetInstance()->GetInnerRoomSize();
	const int iSizeX = (int)vInnerRoomSize.x;
	const int iSizeZ = (int)vInnerRoomSize.z;

	for (int iZ = -5; iZ <= 0; ++iZ)
	{
		for (int iX = -6; iX <= -1; ++iX)
		{
			const int iTileIdx = ((iSizeZ - 1) / 2 - iZ) * iSizeX + (iX + (iSizeX - 1) / 2);

			if (find(m_vecDisabledTileIdx.begin(), m_vecDisabledTileIdx.end(), iTileIdx) != m_vecDisabledTileIdx.end())
				continue;

			AddByName(wstrRoom + L"_Tile_" + to_wstring(iTileIdx));
		}
	}

	/* 4. 버튼 타일 */
	for (CButtonTile* pButtonTile : m_vecButtonTilePhase3)
	{
		m_vecAnamorphicTarget.push_back(pButtonTile);
	}
}

CLiminalRoomLayer* CLiminalRoomLayer::Create(int iRoomIndex)
{
	CLiminalRoomLayer* pLayer = new CLiminalRoomLayer(iRoomIndex);

	if (FAILED(pLayer->Ready_Layer()))
	{
		Safe_Release(pLayer);
		MSG_BOX("[CLiminalRoomLayer] Layer Create Failed");
		return nullptr;
	}

	return pLayer;
}

void CLiminalRoomLayer::Free()
{
	CRoomLayer::Free();
}
