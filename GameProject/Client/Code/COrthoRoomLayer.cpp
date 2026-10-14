#include "pch.h"
#include "COrthoRoomLayer.h"
#include "CRoomLoadingMgr.h"
#include "COrthoCamera.h"
#include "CClientCameraMgr.h"
#include "CDInputMgr.h"
#include "CCubeNetGrass.h"
#include "CCubeNetLava.h"
#include "CCubeNetPlank.h"
#include "CProtoMgr.h"
#include "CGraphicDev.h"
#include "CPlayer.h"
#include "CFlatPlayer.h"
#include "CManagement.h"
#include "CStage.h"
#include "CFlatPlayerMovement.h"
#include "CTimerMgr.h"
#include "CCubeNetBreakable.h"
#include "COrthoPhaseGate.h"
#include "COrthoKey.h"
#include "CCubeNetKeyBlock.h"
#include "COrthoMarker.h"
#include "COrthoEmptyBlocker.h"
#include "CTile.h"
#include "CSpriteTile.h"
#include "CGameMachine.h"
#include "CWeaponSystem.h"

COrthoRoomLayer::COrthoRoomLayer(int iRoomIndex)
	: CRoomLayer(iRoomIndex)
{
}

COrthoRoomLayer::~COrthoRoomLayer()
{
}

HRESULT COrthoRoomLayer::Ready_Layer()
{
	if (FAILED(CRoomLayer::Ready_Layer()))
		return E_FAIL;

	return S_OK;
}

_int COrthoRoomLayer::Update_Layer(_float fTimeDelta)
{
	if (!IsValidUpdateTarget()) return S_OK;

	if (!m_bRegistered)
	{
		m_pOrthoCamera = dynamic_cast<COrthoCamera*>(CClientCameraMgr::GetInstance()->Find_Camera(CLIENT_CAMERA_TYPE::ORTHO));
		m_pPlayer = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene())->GetPlayer();

		m_bRegistered = true;
	}

	/* 게임기 상호작용으로 받아둔 시작 요청 : 충돌 처리 도중이 아니라 여기서 처리 */
	if (m_bPendingStartPhase)
	{
		m_bPendingStartPhase = false;

		m_pPlayer->LockInput(EPlayerInputChannel::PIC_WEAPON);
		/* 미니게임 중에는 무기를 숨김 */
		if (CWeaponSystem* pWeaponSystem = m_pPlayer->GetWeaponSystem())
			pWeaponSystem->SetWeaponVisibility(false);
		/* 2D 플레이어는 시작할 때 켠다 */
		m_pFlatPlayer->Set_IsActive(true);

		/* 기존 타일을 끄고 0.5 미니 타일을 깜 : 이 시점부터 타일 유닛이 0.5 */
		if (FAILED(SpawnMiniTiles()))
			MSG_BOX("[COrthoRoomLayer] SpawnMiniTiles Failed");

		/* 플레이어 카메라 -> 직교 카메라 */
		CClientCameraMgr::GetInstance()->Select_Camera(CLIENT_CAMERA_TYPE::ORTHO);
		m_pOrthoCamera->UpdateView(GetCenterPos() + _vec3{ 0.f, 14.f, -6.f }, GetCenterPos());

		if (FAILED(SpawnPhase(m_iCurrentPhase)))
			MSG_BOX("[COrthoRoomLayer] SpawnPhase Failed");

		m_bPhaseStarted = true;
	}


	/* 페이즈 시작 전(플레이어 카메라)에는 시점 전환 막음 */
	if (m_bPhaseStarted && CDInputMgr::GetInstance()->Key_Down(DIK_R))
	{
		if (m_eMode == PERSP)
		{
			/* -> 직교로 변환 */
			m_pOrthoCamera->ToggleProjectionMode(0.5f);
			m_pOrthoCamera->Orbit(
				GetCenterPos() + _vec3{ 0.f, 14.f, -6.f },
				GetCenterPos() + _vec3{ 0.f, 14.f, 0.f },
				GetCenterPos(),
				0.5f
			);
			m_eMode = ORTHO;
			/* 직교에서는 마커를 보여주지 않음 */
			if (m_pMarker) m_pMarker->SetVisible(false);
			m_pPlayer->LockInput(EPlayerInputChannel::PIC_MOVE);
			m_pFlatPlayer->SetInputEnabled(true);
			CTimerMgr::GetInstance()->ClearGroupTimeScale(CTG_FLATPLAYER);
			CTimerMgr::GetInstance()->SetGroupTimeScale(CTG_PLAYER, 0.f);
		}
		else
		{
			/* -> 원근으로 변환 */
			m_pOrthoCamera->ToggleProjectionMode(0.5f);
			m_pOrthoCamera->Orbit(
				GetCenterPos() + _vec3{ 0.f, 14.f, 0.f },
				GetCenterPos() + _vec3{ 0.f, 14.f, -6.f },
				GetCenterPos(),
				0.5f
			);
			m_eMode = PERSP;
			/* 원근으로 돌아오면 들고 있는 중일 때만 마커 표시 */
			if (m_pMarker) m_pMarker->SetVisible(m_bIsGrabbing);
			m_pPlayer->UnlockInput(EPlayerInputChannel::PIC_MOVE);
			m_pFlatPlayer->SetInputEnabled(false);
			CTimerMgr::GetInstance()->ClearGroupTimeScale(CTG_PLAYER);
			CTimerMgr::GetInstance()->SetGroupTimeScale(CTG_FLATPLAYER, 0.f);
		}
		//m_pOrthoCamera->Orbit(
		//	GetCenterPos() + _vec3{ 0.f, 0.f, -10.f },
		//	GetCenterPos() + _vec3{ 0.f, 10.f, 0.f },
		//	GetCenterPos(),
		//	2.f
		//);
	}

	/* 두 플레이어가 동시에 게이트에 닿은 순간 한 번만 클리어 */
	const bool bBothOnGate = m_bPlayerOnGate && m_bFlatPlayerOnGate;
	if (bBothOnGate && !m_bWasBothOnGate)
	{
		ClearCurrentPhase();
	}
	m_bWasBothOnGate = bBothOnGate;

	_int iExit = CRoomLayer::Update_Layer(fTimeDelta);

	return iExit;
}

void COrthoRoomLayer::LateUpdate_Layer(_float fTimeDelta)
{
	if (!IsValidUpdateTarget()) return;

	CRoomLayer::LateUpdate_Layer(fTimeDelta);
}

HRESULT COrthoRoomLayer::SpawnRoom()
{
	if (FAILED(CRoomLayer::SpawnRoom()))
		return E_FAIL;

	LPDIRECT3DDEVICE9 pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();

	const _vec3 vCenter = GetCenterPos();

	/* 2D 플레이어 : 방 생성 시 만들어 두고, 게임기로 시작하기 전까지는 꺼둠 */
	m_pFlatPlayer = CFlatPlayer::Create(pDevice);
	if (nullptr == m_pFlatPlayer)
		return E_FAIL;
	if (FAILED(Add_GameObject(L"Flat_Player", m_pFlatPlayer)))
		return E_FAIL;
	m_pFlatPlayer->Set_IsActive(false);

	/* 내려놓기 위치 마커 : 방마다 하나 */
	m_pMarker = COrthoMarker::Create(pDevice);
	if (FAILED(Add_GameObject(L"Room_" + to_wstring(m_iRoomIndex) + L"_Marker", m_pMarker)))
		return E_FAIL;

	/* 게임기 : 방 가운데. 함수를 등록하므로 씬 타입은 쓰이지 않음 */
	m_pGameMachine = CGameMachine::Create(pDevice, ESceneType::SCENE_NONE);
	if (nullptr == m_pGameMachine)
		return E_FAIL;

	/* 상호작용 시 : 시작 요청만 받아두고 게임기는 바로 제거 */
	m_pGameMachine->RegisterFunction([this]()
		{
			m_bPendingStartPhase = true;

			if (m_pGameMachine)
			{
				m_pGameMachine->Set_Dead(true);
				m_pGameMachine = nullptr;
			}
		});

	if (FAILED(Add_GameObject(L"Room_" + to_wstring(m_iRoomIndex) + L"_GameMachine", m_pGameMachine)))
		return E_FAIL;

	CTransform* pMachineTransform = dynamic_cast<CTransform*>(m_pGameMachine->Get_Component(ID_DYNAMIC, L"Com_Transform"));
	if (pMachineTransform)
		pMachineTransform->Set_Pos(vCenter.x, 0.f, vCenter.z);

	return S_OK;
}

HRESULT COrthoRoomLayer::SpawnPhase(int iPhase)
{
	LPDIRECT3DDEVICE9 pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();

	/* 칸 수는 현재 타일 유닛 기준 (미니 타일이 깔린 뒤에 호출되므로 0.5 격자) */
	const _vec3 vInnerRoomSize = CRoomLoadingMgr::GetInstance()->GetInnerRoomSize();
	const float fUnit = GetTileUnit();
	const int iSizeX = (int)roundf(vInnerRoomSize.x / fUnit);
	const int iSizeZ = (int)roundf(vInnerRoomSize.z / fUnit);
	const _vec3 vCenter = GetCenterPos();

	/* 큐브 피벗이 중심이면 블럭 크기 절반, 바닥이면 0.f (블럭 크기 = 미니 타일 0.5) */
	const float fBlockY = 0.5f * m_fMiniTileUnit;

	auto SpawnBlock = [&](CGameObject* pObj, const wstring& wstrTag, int iTileX, int iTileZ) -> HRESULT
		{
			if (nullptr == pObj) return E_FAIL;
			if (iTileX < 0 || iTileX >= iSizeX || iTileZ < 0 || iTileZ >= iSizeZ)
			{
				Safe_Release(pObj);
				return S_OK;
			}

			wstring wstrName = L"Room_" + to_wstring(m_iRoomIndex) + L"_" + wstrTag
				+ L"_" + to_wstring(iTileZ) + L"_" + to_wstring(iTileX);

			if (FAILED(Add_GameObject(wstrName, pObj)))
				return E_FAIL;

			const _vec3 vTilePos = GetTileCenterPos(TTileIdx{ iTileZ, iTileX });

			CTransform* pTransformCom = dynamic_cast<CTransform*>(pObj->Get_Component(ID_DYNAMIC, L"Com_Transform"));
			if (pTransformCom)
				pTransformCom->Set_Pos(vTilePos.x, fBlockY, vTilePos.z);

			// if (IRayTestable* p = dynamic_cast<IRayTestable*>(pObj)) m_vecRayTestable.push_back(p);
			if (IVerticalTerrain* p = dynamic_cast<IVerticalTerrain*>(pObj))         m_vecVerticalTerrain.push_back(p);

			RegisterPhaseObject(pObj);

			return S_OK;
		};

	/* 페이즈마다 3D 플레이어 / 2D 플레이어 시작 위치 */
	_vec3 vPlayerPos = m_pPlayer->GetTransform()->Get_Info_Value(INFO_POS);
	_vec3 vFlatPlayerPos = vCenter;

	switch (iPhase)
	{
	case 0:
	{
		/* 1. 맨 아랫줄 : Lava 한 줄 전체 */
		for (int x = 0; x < iSizeX; ++x)
		{
			if (FAILED(SpawnBlock(CCubeNetLava::Create(pDevice), L"TestLava", x, iSizeZ - 1)))
				return E_FAIL;
		}

		/* 2. 중간 줄 : Grass 2~3칸 덩어리 */
		{
			const int iMidZ = iSizeZ / 2;
			const int iMidX = iSizeX / 2;

			/* {시작 x, 길이} */
			const pair<int, int> GrassGroups[] = {
				//{ 1,          2 },   // 왼쪽 2칸
				//{ iMidX - 1,  3 },   // 가운데 3칸
				{ iSizeX - 3, 2 },   // 오른쪽 2칸
			};

			for (const auto& [iStartX, iLen] : GrassGroups)
			{
				for (int x = iStartX; x < iStartX + iLen; ++x)
				{
					if (FAILED(SpawnBlock(CCubeNetGrass::Create(pDevice), L"TestGrass", x, iMidZ)))
						return E_FAIL;
				}
			}
		}

		/* 3. 위쪽 : Plank 4개 띄엄띄엄 */
		{
			const int iTopZ = 1;
			for (int k = 0; k < 4; ++k)
			{
				int x = iSizeX * (k + 1) / 5;  // 가로를 5등분한 경계 위치
				if (FAILED(SpawnBlock(CCubeNetPlank::Create(pDevice), L"TestPlank", x, iTopZ)))
					return E_FAIL;
			}
		}

		const int iBreakZ = iSizeZ / 2 + 1;
		const int iStartX = iSizeX / 2 - 2;
		const int iLen = 4;

		CCubeNetBreakable* pHead = nullptr;

		for (int x = iStartX; x < iStartX + iLen; ++x)
		{
			/* SpawnBlock은 범위 밖이면 Release 후 S_OK를 반환하므로,
			   댕글링 포인터가 head 목록에 들어가지 않게 미리 거른다 */
			if (x < 0 || x >= iSizeX || iBreakZ >= iSizeZ - 1)
				continue;

			CCubeNetBreakable* pBreak = CCubeNetBreakable::Create(pDevice);
			if (FAILED(SpawnBlock(pBreak, L"TestBreakable", x, iBreakZ)))
				return E_FAIL;

			if (!pHead) pHead = pBreak;
			else        pBreak->SetHead(pHead);

			pBreak->m_OnDead.AddBinding(GetToken(), [this](IVerticalTerrain* const pTerrain) { RemoveDeadTerrain(pTerrain); });
		}

		/* 열쇠 블럭 : 가운데 줄 비어 있는 3칸 */
		{
			const int iMidZ = iSizeZ / 2;
			const int iMidX = iSizeX / 2;

			for (int x = iMidX - 1; x < iMidX + 2; ++x)
			{
				/* 범위 밖이면 SpawnBlock이 Release하므로 미리 거른다 */
				if (x < 0 || x >= iSizeX)
					continue;

				CCubeNetKeyBlock* pKeyBlock = CCubeNetKeyBlock::Create(pDevice);
				if (FAILED(SpawnBlock(pKeyBlock, L"KeyBlock", x, iMidZ)))
					return E_FAIL;

				m_vecKeyBlock.push_back(pKeyBlock);
			}
		}

		/* 열쇠 : 왼쪽 위 구석, 바닥에 붙여서 배치 */
		{
			COrthoKey* pKey = COrthoKey::Create(pDevice);
			if (FAILED(SpawnBlock(pKey, L"Key", 0, 0)))
				return E_FAIL;

			_vec3 vKeyPos;
			pKey->GetTransform()->Get_Info(INFO_POS, &vKeyPos);
			vKeyPos.y = 0.5f;
			pKey->GetTransform()->Set_Pos(vKeyPos);

			++m_iKeyCount;
		}

		/* 뚫린 바닥 (테스트) */
		if (FAILED(SpawnEmptyTile(1, 2)))
			return E_FAIL;

		/* 페이즈 게이트 */
		if (FAILED(SpawnBlock(COrthoPhaseGate::Create(pDevice), L"PhaseGate", 0, iSizeZ / 2)))
			return E_FAIL;

		/* 시작 위치 */
		const _vec3 vPlayerTile = GetTileCenterPos(TTileIdx{ iSizeZ - 2, iSizeX / 2 });
		vPlayerPos.x = vPlayerTile.x;
		vPlayerPos.z = vPlayerTile.z;
		vFlatPlayerPos = vCenter + _vec3{ 5.f, 0.03f, 3.f }; /* z fighting 방지 위해 표면에서 조금 띄워둠 */
		break;
	}
	case 1:
	case 2:
	{
		/* 임시 : 로직 확인용으로 페이즈 게이트만 좌하단에 배치 */
		if (FAILED(SpawnBlock(COrthoPhaseGate::Create(pDevice), L"PhaseGate", 0, iSizeZ - 1)))
			return E_FAIL;
		break;
	}
	default:
		break;
	}

	m_pPlayer->GetTransform()->Set_Pos(vPlayerPos);
	m_pFlatPlayer->GetTransform()->Set_Pos(vFlatPlayerPos);

	return S_OK;
}

void COrthoRoomLayer::GetFlatBounds(_vec2& vMin, _vec2& vMax)
{
	const _vec3 vInnerRoomSize = CRoomLoadingMgr::GetInstance()->GetInnerRoomSize();
	const _vec3 vCenter = GetCenterPos();

	vMin = { vCenter.x - vInnerRoomSize.x * 0.5f, vCenter.z - vInnerRoomSize.z * 0.5f };
	vMax = { vCenter.x + vInnerRoomSize.x * 0.5f, vCenter.z + vInnerRoomSize.z * 0.5f };
}

void COrthoRoomLayer::RemoveDeadTerrain(IVerticalTerrain* pTerrain)
{
	auto iter = remove(m_vecVerticalTerrain.begin(), m_vecVerticalTerrain.end(), pTerrain);
	m_vecVerticalTerrain.erase(iter, m_vecVerticalTerrain.end());
}

void COrthoRoomLayer::OnKeyAcquired()
{
	--m_iKeyCount;

	/* 사후 체크 : 페이즈 안의 키를 모두 모았으면 열쇠 블럭 파괴 */
	if (m_iKeyCount <= 0)
	{
		m_iKeyCount = 0;
		DestroyKeyBlocks();
	}
}

void COrthoRoomLayer::SetGrabbing(bool bFlag)
{
	m_bIsGrabbing = bFlag;

	if (m_pMarker)
		m_pMarker->SetVisible(m_bIsGrabbing && m_eMode == PERSP);
}

void COrthoRoomLayer::UpdateMarkerPos(const _vec3& vHeldPos)
{
	if (nullptr == m_pMarker) return;

	_vec3 vMarkerPos = GetNearestTileCenterPos(vHeldPos);
	vMarkerPos.y = 0.02f; /* z fighting 방지 위해 바닥에서 조금 띄움 */
	m_pMarker->GetTransform()->Set_Pos(vMarkerPos);
}

void COrthoRoomLayer::RequestPlaceOrthoObject(CCubeNet* pObj, const _vec3& vHeldPos)
{
	if (nullptr == pObj) return;

	const TTileIdx tIdx = GetNearestTileIndex(vHeldPos);
	const int iIndex = ToFlatIndex(tIdx);
	_vec3 vPos = GetTileCenterPos(tIdx);

	if (0 <= iIndex && iIndex < (int)m_vecEmptyTile.size() && m_vecEmptyTile[iIndex])
	{
		/* 뚫린 바닥 : 블럭이 구멍을 메운다 */
		vPos.y -= 0.5f * m_fMiniTileUnit;
		m_vecEmptyTile[iIndex] = false;

		/* 메운 플랭크는 다시 들 수 없게 */
		if (CCubeNetPlank* pPlank = dynamic_cast<CCubeNetPlank*>(pObj))
			pPlank->LockInHole();

		if (m_vecEmptyBlocker[iIndex])
		{
			m_vecEmptyBlocker[iIndex]->Set_Dead(true);
			m_vecEmptyBlocker[iIndex] = nullptr;
		}
	}
	else
	{
		/* 일반 바닥 */
		vPos.y += 0.5f * m_fMiniTileUnit;
	}

	pObj->GetTransform()->Set_Pos(vPos);
}

HRESULT COrthoRoomLayer::SpawnEmptyTile(int iTileX, int iTileZ)
{
	const TTileIdx tIdx{ iTileZ, iTileX };
	const int iIndex = ToFlatIndex(tIdx);
	if (iIndex < 0 || iIndex >= (int)m_vecEmptyTile.size()) return S_OK;

	LPDIRECT3DDEVICE9 pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();

	COrthoEmptyBlocker* pBlocker = COrthoEmptyBlocker::Create(pDevice);
	if (nullptr == pBlocker) return E_FAIL;

	wstring wstrName = L"Room_" + to_wstring(m_iRoomIndex) + L"_EmptyBlocker_"
		+ to_wstring(iTileZ) + L"_" + to_wstring(iTileX);
	if (FAILED(Add_GameObject(wstrName, pBlocker)))
		return E_FAIL;

	/* 큐브넷 블럭과 같은 높이 기준 (SpawnBlock의 fBlockY) */
	_vec3 vPos = GetTileCenterPos(tIdx);
	vPos.y = 0.5f * m_fMiniTileUnit;
	pBlocker->GetTransform()->Set_Pos(vPos);

	/* 같은 칸 타일은 꺼서 빈 공간처럼 보이게 */
	if (CTile* pTile = GetTileFromIndex2D(tIdx))
	{
		pTile->Set_IsActive(false);
		m_vecDisabledTile.push_back(pTile);
	}

	m_vecEmptyTile[iIndex] = true;
	m_vecEmptyBlocker[iIndex] = pBlocker;
	RegisterPhaseObject(pBlocker);

	return S_OK;
}

void COrthoRoomLayer::DestroyKeyBlocks()
{
	for (CCubeNetKeyBlock* pKeyBlock : m_vecKeyBlock)
	{
		RemoveDeadTerrain(pKeyBlock);
		pKeyBlock->Set_Dead(true);
	}
	m_vecKeyBlock.clear();
}

void COrthoRoomLayer::ClearCurrentPhase()
{
	if (m_iCurrentPhase >= m_iLastPhase)
	{
		/* 마지막 페이즈 : 페이즈 정리 -> 미니게임 종료 -> 방 클리어 요청 */
		CleanCurrentPhase();
		EndOrthoGame();

		/* 방의 클리어 조건에 ManualClear가 있으면 이 요청으로 방이 클리어된다 */
		TRoomEventCtx t{ ERoomEventType::ROOM_MANUALCLEAR_REQUEST };
		m_OnRoomEvent.Broadcast(t);
		return;
	}

	/* 다음 페이즈 : 현재 페이즈 정리 후 스폰 */
	CleanCurrentPhase();
	++m_iCurrentPhase;

	if (FAILED(SpawnPhase(m_iCurrentPhase)))
		MSG_BOX("[COrthoRoomLayer] SpawnPhase Failed");
}

void COrthoRoomLayer::RegisterPhaseObject(CGameObject* pObj)
{
	if (nullptr == pObj) return;
	m_vecPhaseObject.push_back({ pObj, pObj->GetToken() });
}

void COrthoRoomLayer::CleanCurrentPhase()
{
	/* 플레이어가 들고 있거나 후보로 잡고 있는 블럭이 곧 죽으므로 먼저 놓게 함 */
	m_pPlayer->ResetOrthoInteraction();
	SetGrabbing(false);

	/* 페이즈 전용 오브젝트 제거 : 이미 죽어서 해제된 것(열쇠, 부서진 블럭 등)은 건너뜀 */
	for (auto& [pObj, wToken] : m_vecPhaseObject)
	{
		if (!wToken.expired())
			pObj->Set_Dead(true);
	}
	m_vecPhaseObject.clear();

	/* 지형 목록 : 죽을 블럭을 가리키지 않도록 비움 */
	m_vecVerticalTerrain.clear();

	/* 열쇠 */
	m_iKeyCount = 0;
	m_vecKeyBlock.clear();

	/* 뚫린 바닥 : 꺼둔 타일 복구, 상태 초기화 */
	for (CTile* pTile : m_vecDisabledTile)
		pTile->Set_IsActive(true);
	m_vecDisabledTile.clear();
	fill(m_vecEmptyTile.begin(), m_vecEmptyTile.end(), false);
	fill(m_vecEmptyBlocker.begin(), m_vecEmptyBlocker.end(), nullptr);

	/* 게이트 : 죽은 게이트는 Exit가 오지 않으므로 직접 끔 */
	m_bPlayerOnGate = false;
	m_bFlatPlayerOnGate = false;
	m_bWasBothOnGate = false;
}

void COrthoRoomLayer::EndOrthoGame()
{
	if (!m_bPhaseStarted) return;
	m_bPhaseStarted = false;

	/* 직교 상태에서 끝났으면 원근으로 되돌려 둠 (다음에 이 카메라를 쓸 때 상태가 맞도록) */
	if (m_eMode == ORTHO)
	{
		m_pOrthoCamera->ToggleProjectionMode(0.5f);
		m_eMode = PERSP;
	}

	/* 카메라 : 직교 카메라 -> 플레이어 카메라 */
	CClientCameraMgr::GetInstance()->Select_Camera(CLIENT_CAMERA_TYPE::PLAYER);

	/* 3D 플레이어 입력 복구 */
	m_pPlayer->UnlockInput(EPlayerInputChannel::PIC_MOVE);
	m_pPlayer->UnlockInput(EPlayerInputChannel::PIC_WEAPON);
	if (CWeaponSystem* pWeaponSystem = m_pPlayer->GetWeaponSystem())
		pWeaponSystem->SetWeaponVisibility(true);

	/* 타임스케일 복구 */
	CTimerMgr::GetInstance()->ClearGroupTimeScale(CTG_PLAYER);
	CTimerMgr::GetInstance()->ClearGroupTimeScale(CTG_FLATPLAYER);

	/* 2D 플레이어는 다시 꺼둠 */
	m_pFlatPlayer->SetInputEnabled(false);
	m_pFlatPlayer->Set_IsActive(false);

	/* 마커 숨김 */
	m_bIsGrabbing = false;
	if (m_pMarker) m_pMarker->SetVisible(false);

	/* 미니 타일 제거 + 기존 타일 복구 : 이 시점부터 타일 유닛이 다시 1 */
	RemoveMiniTiles();
}

HRESULT COrthoRoomLayer::SpawnMiniTiles()
{
	if (m_bMiniTileActive) return S_OK;

	LPDIRECT3DDEVICE9 pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();

	const _vec3 vInnerRoomSize = CRoomLoadingMgr::GetInstance()->GetInnerRoomSize();
	const int iOriginSizeX = (int)vInnerRoomSize.x;
	const int iOriginSizeZ = (int)vInnerRoomSize.z;

	/* 1. 기존 타일 : 끄고 따로 들고 있음. 아직 유닛 1 상태이므로 기본 조회 사용
	      (미니 타일이 같은 텍스쳐를 쓰도록 인덱스 순서대로도 보관) */
	vector<CTile*> vecOriginByIndex(iOriginSizeX * iOriginSizeZ, nullptr);
	m_vecOriginTile.clear();
	for (int iRow = 0; iRow < iOriginSizeZ; ++iRow)
	{
		for (int iCol = 0; iCol < iOriginSizeX; ++iCol)
		{
			CTile* pTile = CRoomLayer::GetTileFromIndex2D(TTileIdx{ iRow, iCol });
			if (nullptr == pTile) continue;

			pTile->Set_IsActive(false);
			m_vecOriginTile.push_back(pTile);
			vecOriginByIndex[iRow * iOriginSizeX + iCol] = pTile;
		}
	}

	/* 2. 여기서부터 타일 유닛 0.5 : GetTileCenterPos 등이 미니 격자 기준으로 계산됨 */
	m_bMiniTileActive = true;

	const int iSizeX = (int)roundf(vInnerRoomSize.x / m_fMiniTileUnit);
	const int iSizeZ = (int)roundf(vInnerRoomSize.z / m_fMiniTileUnit);

	m_vecMiniTile.assign(iSizeX * iSizeZ, nullptr);

	for (int iRow = 0; iRow < iSizeZ; ++iRow)
	{
		for (int iCol = 0; iCol < iSizeX; ++iCol)
		{
			/* 이 미니 칸을 덮고 있던 기존 타일의 텍스쳐/오염 저항을 그대로 씀 */
			const int iOriginRow = min((int)(((float)iRow + 0.5f) * m_fMiniTileUnit), iOriginSizeZ - 1);
			const int iOriginCol = min((int)(((float)iCol + 0.5f) * m_fMiniTileUnit), iOriginSizeX - 1);

			int iTextureIdx = m_tBiomeInfo.iDefaultTileIndex;
			bool bResistContamination = false;
			CTile* pOrigin = vecOriginByIndex[iOriginRow * iOriginSizeX + iOriginCol];
			if (pOrigin && pOrigin->GetType() == ETileType::SPRITE)
			{
				CSpriteTile* pOriginSprite = static_cast<CSpriteTile*>(pOrigin);
				iTextureIdx = pOriginSprite->GetTextureIdx();
				bResistContamination = pOriginSprite->GetResistContamination();
			}

			const int iIndex = iRow * iSizeX + iCol;

			CSpriteTile* pMiniTile = CSpriteTile::Create(pDevice, iIndex, iTextureIdx, bResistContamination);
			if (nullptr == pMiniTile)
				return E_FAIL;

			/* CTile 생성자는 기본 격자로 2D 인덱스를 계산하므로 미니 격자 인덱스로 덮어씀 */
			pMiniTile->SetPositionIndex2D(TTileIdx{ iRow, iCol });

			wstring wstrName = L"Room_" + to_wstring(m_iRoomIndex) + L"_MiniTile_" + to_wstring(iIndex);
			if (FAILED(Add_GameObject(wstrName, pMiniTile)))
				return E_FAIL;

			const _vec3 vPos = GetTileCenterPos(TTileIdx{ iRow, iCol });
			pMiniTile->GetTransform()->Set_Pos(vPos.x, 0.f, vPos.z);
			pMiniTile->GetTransform()->Set_Scale(m_fMiniTileUnit, 1.f, m_fMiniTileUnit);

			m_vecMiniTile[iIndex] = pMiniTile;
		}
	}

	/* 3. 뚫린 바닥 상태 컨테이너 : 미니 격자 기준, 처음엔 전부 막힌 바닥 */
	m_iTileCountX = iSizeX;
	m_vecEmptyTile.assign(iSizeX * iSizeZ, false);
	m_vecEmptyBlocker.assign(iSizeX * iSizeZ, nullptr);

	return S_OK;
}

void COrthoRoomLayer::RemoveMiniTiles()
{
	if (!m_bMiniTileActive) return;

	/* 미니 타일 제거 */
	for (CTile* pTile : m_vecMiniTile)
	{
		if (pTile) pTile->Set_Dead(true);
	}
	m_vecMiniTile.clear();

	/* 여기서부터 타일 유닛 다시 1 */
	m_bMiniTileActive = false;

	/* 기존 타일 다시 켬 */
	for (CTile* pTile : m_vecOriginTile)
		pTile->Set_IsActive(true);
	m_vecOriginTile.clear();

	/* 뚫린 바닥 컨테이너는 미니 격자 기준이었으므로 비움 */
	m_iTileCountX = 0;
	m_vecEmptyTile.clear();
	m_vecEmptyBlocker.clear();
}

CTile* COrthoRoomLayer::GetTileFromIndex2D(const TTileIdx& tIdx)
{
	if (!m_bMiniTileActive)
		return CRoomLayer::GetTileFromIndex2D(tIdx);

	if (0 > tIdx.iCol || tIdx.iCol >= m_iTileCountX || 0 > tIdx.iRow) return nullptr;

	const int iIndex = ToFlatIndex(tIdx);
	if (iIndex >= (int)m_vecMiniTile.size()) return nullptr;

	return m_vecMiniTile[iIndex];
}

COrthoRoomLayer* COrthoRoomLayer::Create(int iRoomIndex)
{
	COrthoRoomLayer* pLayer = new COrthoRoomLayer(iRoomIndex);

	if (FAILED(pLayer->Ready_Layer()))
	{
		Safe_Release(pLayer);
		MSG_BOX("[COrthoRoomLayer] Layer Create Failed");
		return nullptr;
	}

	return pLayer;
}

void COrthoRoomLayer::Free()
{
	CRoomLayer::Free();
}
