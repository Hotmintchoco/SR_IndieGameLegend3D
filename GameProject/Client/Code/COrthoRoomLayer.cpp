#include "pch.h"
#include "COrthoRoomLayer.h"
#include "CRoomLoadingMgr.h"
#include "COrthoCamera.h"
#include "CClientCameraMgr.h"
#include "CCinematicCamera.h"
#include "CPlayerMovement.h"
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
#include "CUI_PhaseClear.h"
#include "CUI_OrthoIntro.h"
#include "CUI_PhaseNumber.h"
#include "CTile.h"
#include "CWall.h"
#include "CSpriteTile.h"
#include "COrthoSpriteTile.h"
#include "CGameMachine.h"
#include "CWeaponSystem.h"
#include "CSoundMgr.h"

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

	/* 미니 타일 배경 애니메이션 공용 시간 */
	if (m_bMiniTileActive)
		m_fMiniTileTime += fTimeDelta;

	/* 게임기 상호작용으로 받아둔 시작 요청 : 충돌 처리 도중이 아니라 여기서 처리 (바로 시작하지 않고 입장 연출부터) */
	if (m_bPendingStartPhase)
	{
		m_bPendingStartPhase = false;
		StartEnterDirecting();
	}

	/* 입장/퇴장 카메라 연출 진행 */
	if (m_eDirecting != EDirecting::NONE)
		UpdateDirecting(fTimeDelta);


	/* 페이즈 시작 전(플레이어 카메라)에는 시점 전환 막음 */
	if (m_bPhaseStarted && !m_bTransitionInputLocked && CDInputMgr::GetInstance()->Key_Down(DIK_R))
	{
		SetGameMode((m_eMode == PERSP) ? ORTHO : PERSP);
	}

	/* 클리어 UI 재생이 끝났으면 후속 처리 (커튼 시작 또는 미니게임 정리) */
	if (m_bClearUIFinished)
	{
		m_bClearUIFinished = false;
		OnPhaseClearUIFinished();
	}

	/* 페이즈 전환 커튼 진행 */
	if (IsCurtainPlaying())
		UpdateCurtain(fTimeDelta);

	/* 용암 등으로 요청된 리셋 : 충돌 처리 다음 프레임에 수행 (전환 중에는 무시)
	   같은 프레임 위에서 클리어 UI가 끝나 미니게임이 정리됐을 수 있으므로(미니 타일 제거 후 SpawnPhase -> 크래시) 진행 중일 때만 */
	if (m_bPendingResetPhase)
	{
		m_bPendingResetPhase = false;
		if (m_bPhaseStarted && !IsTransitioning())
			ResetCurrentPhase();
	}

	/* 두 플레이어가 동시에 게이트에 닿은 순간 한 번만 클리어 */
	const bool bBothOnGate = m_bPlayerOnGate && m_bFlatPlayerOnGate;
	if (bBothOnGate && !m_bWasBothOnGate && !IsTransitioning())
	{
		ClearCurrentPhase();
	}
	m_bWasBothOnGate = bBothOnGate;

	if (m_bPhaseStarted && !IsTransitioning() && CDInputMgr::GetInstance()->Key_Down(DIK_0))
	{
		ClearCurrentPhase();
	}

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

	/* 페이즈 클리어 UI : 꺼진 상태로 만들어 두고 클리어 시 Play */
	m_pPhaseClearUI = CUI_PhaseClear::Create(pDevice);
	if (nullptr == m_pPhaseClearUI)
		return E_FAIL;
	if (FAILED(Add_GameObject(L"Room_" + to_wstring(m_iRoomIndex) + L"_PhaseClearUI", m_pPhaseClearUI)))
		return E_FAIL;
	/* 재생이 끝나면 다음 레이어 업데이트에서 후속 처리 */
	m_pPhaseClearUI->m_OnPlayFinished.AddBinding(GetToken(), [this]() { m_bClearUIFinished = true; });

	/* 입장 인트로 UI : 꺼진 상태로 만들어 두고 입장 Orbit이 끝나면 Play, 아무 키로 끝나면 게임 시작 */
	m_pIntroUI = CUI_OrthoIntro::Create(pDevice);
	if (nullptr == m_pIntroUI)
		return E_FAIL;
	if (FAILED(Add_GameObject(L"Room_" + to_wstring(m_iRoomIndex) + L"_IntroUI", m_pIntroUI)))
		return E_FAIL;
	m_pIntroUI->m_OnPlayFinished.AddBinding(GetToken(), [this]() { m_bIntroFinished = true; });

	/* 좌상단 페이즈 표시 UI : 꺼진 상태로 만들어 두고 미니게임 시작 시 Show */
	m_pPhaseNumberUI = CUI_PhaseNumber::Create(pDevice);
	if (nullptr == m_pPhaseNumberUI)
		return E_FAIL;
	if (FAILED(Add_GameObject(L"Room_" + to_wstring(m_iRoomIndex) + L"_PhaseNumberUI", m_pPhaseNumberUI)))
		return E_FAIL;
	/* 전체 페이즈 수는 고정 (표시는 1부터) */
	m_pPhaseNumberUI->SetTotalPhase(m_iLastPhase + 1);

	/* 게임기 : 방 가운데. 함수를 등록하므로 씬 타입은 쓰이지 않음 */
	m_pGameMachine = CGameMachine::Create(pDevice, ESceneType::SCENE_NONE);
	if (nullptr == m_pGameMachine)
		return E_FAIL;

	/* 상호작용 시 : 시작 요청만 받아둠 (게임기는 입장 연출이 끝나고 게임이 시작될 때 제거) */
	m_pGameMachine->RegisterFunction([this]()
		{
			/* 이미 입장 연출 중이거나 게임 중이면 무시 */
			if (m_bPendingStartPhase || m_eDirecting != EDirecting::NONE || m_bPhaseStarted)
				return;

			m_bPendingStartPhase = true;
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

	/* 좌상단 페이즈 표시 갱신 (시작/다음 페이즈/재시작 모두 여기를 지남, 표시는 1부터) */
	if (m_pPhaseNumberUI)
		m_pPhaseNumberUI->SetCurrentPhase(iPhase + 1);

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
			/* 큐브넷 블럭은 칸을 차지 (게이트/열쇠는 차지하지 않음) */
			if (CCubeNet* p = dynamic_cast<CCubeNet*>(pObj))                         SetTileOccupant(ToFlatIndex(TTileIdx{ iTileZ, iTileX }), p);

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
		/* 원작 0페이즈 구성 (미니 격자 26 x 22 기준, 행은 위에서 아래로)
		   좌측 : 포탈 + 바로 아래 잔디 발판
		   포탈 우측 : 천장~바닥 세로로 뚫린 바닥 + 그 옆 가로로 뚫린 바닥
		   우측 : 나무 플랭크 4개 (2D 플레이어가 그 위에서 시작)
		   맨 아래 : 용암 */
		const int iLedgeZ = 9;			/* 잔디/뚫린 가로줄/플랭크가 놓이는 줄 */
		const int iHoleX = 3;			/* 세로로 뚫린 열 */
		const int iPlankStartX = 18;	/* 플랭크 시작 열 (4개) */
		const int iLavaRows = 4;		/* 용암 두께 (맨 아래부터) */

		/* 1. 맨 아래 네 줄 : 용암 (세로 구멍 열은 비움) */
		for (int z = iSizeZ - iLavaRows; z < iSizeZ; ++z)
		{
			for (int x = 0; x < iSizeX; ++x)
			{
				if (x == iHoleX) continue;
				if (FAILED(SpawnBlock(CCubeNetLava::Create(pDevice), L"Lava", x, z)))
					return E_FAIL;
			}
		}

		/* 2. 포탈 아래 잔디 발판 : 포탈 폭(2칸)보다 조금 넓게 */
		for (int x = 0; x < iHoleX; ++x)
		{
			if (FAILED(SpawnBlock(CCubeNetGrass::Create(pDevice), L"Grass", x, iLedgeZ)))
				return E_FAIL;
		}

		/* 3. 뚫린 바닥 : 세로 한 줄 (천장~바닥) */
		for (int z = 0; z < iSizeZ; ++z)
		{
			if (FAILED(SpawnEmptyTile(iHoleX, z)))
				return E_FAIL;
		}

		/* 4. 뚫린 바닥 : 세로 구멍 옆 가로 4칸 */
		for (int x = iHoleX + 1; x < iHoleX + 5; ++x)
		{
			if (FAILED(SpawnEmptyTile(x, iLedgeZ)))
				return E_FAIL;
		}

		/* 5. 우측 나무 플랭크 4개 */
		for (int x = iPlankStartX; x < iPlankStartX + 4; ++x)
		{
			if (FAILED(SpawnBlock(CCubeNetPlank::Create(pDevice), L"Plank", x, iLedgeZ)))
				return E_FAIL;
		}

		/* 6. 클리어 포탈 : 잔디 발판 바로 위 */
		if (FAILED(SpawnBlock(COrthoPhaseGate::Create(pDevice), L"PhaseGate", 1, iLedgeZ - 2)))
			return E_FAIL;

		/* 시작 위치 : 2D 플레이어는 플랭크 위, 3D 플레이어는 그 아래쪽 바닥 */
		const _vec3 vFlatTile = GetTileCenterPos(TTileIdx{ iLedgeZ - 1, iPlankStartX + 2 });
		vFlatPlayerPos = _vec3{ vFlatTile.x, 0.1f, vFlatTile.z }; /* z fighting 방지 위해 표면에서 조금 띄워둠 */

		const _vec3 vPlayerTile = GetTileCenterPos(TTileIdx{ iLedgeZ + 3, iPlankStartX + 4 });
		vPlayerPos.x = vPlayerTile.x;
		vPlayerPos.z = vPlayerTile.z;
		break;
	}
	case 1:
	{
		/* 원작 1페이즈 구성 (미니 격자 26 x 22 기준, 행은 위에서 아래로)
		   좌상단 : 클리어 포탈
		   가운데 위 : 열쇠 블럭 세로 기둥 (천장~뚫린 띠 바로 위)
		   가운데 : 가로로 넓게 뚫린 바닥 띠 + 우측 플레이어 쪽으로 이어지는 얇은 띠
		   부서지는 블럭 'ㄱ' 모양 3개 (윗변은 2D 발판, 기둥은 용암까지)
		   우측 : 잔디 발판 (2D 플레이어 시작)
		   아래 : 좌하단 나무 플랭크, 가운데 열쇠, 맨 아래 용암 */
		const int iBarZ = 12;			/* 'ㄱ' 윗변 / 우측 잔디 발판 줄 */
		const int iLavaRows = 2;		/* 용암 두께 (맨 아래부터) */
		const int iLegEndZ = iSizeZ - iLavaRows - 1;	/* 'ㄱ' 기둥 끝 (용암 바로 위) */
		const int iHoleTopZ = 10;		/* 뚫린 띠 시작 줄 */
		const int iHoleBottomZ = 15;	/* 뚫린 띠 끝 줄 */
		const int iHoleEndX = 20;		/* 뚫린 띠 마지막 열 */
		const int iStripZ = 11;			/* 우측 얇은 띠 줄 */
		const int iKeyBlockX = 16;		/* 열쇠 블럭 기둥 열 */

		/* 'ㄱ' 모양 : 윗변 시작 열 / 윗변 길이 (기둥은 시작 열) */
		struct TBreakableShape { int iStartX; int iWidth; };
		const TBreakableShape tShapes[] = { { 6, 3 }, { 12, 4 }, { 19, 3 } };

		/* 부서지는 블럭이 놓일 칸 : 뚫린 바닥에서 제외하기 위해 미리 표시 */
		vector<bool> vecSolid(iSizeX * iSizeZ, false);
		for (const TBreakableShape& tShape : tShapes)
		{
			for (int x = tShape.iStartX; x < tShape.iStartX + tShape.iWidth; ++x)
				vecSolid[iBarZ * iSizeX + x] = true;
			for (int z = iBarZ + 1; z <= iLegEndZ; ++z)
				vecSolid[z * iSizeX + tShape.iStartX] = true;
		}

		/* 1. 맨 아래 두 줄 : 용암 */
		for (int z = iSizeZ - iLavaRows; z < iSizeZ; ++z)
		{
			for (int x = 0; x < iSizeX; ++x)
			{
				if (FAILED(SpawnBlock(CCubeNetLava::Create(pDevice), L"Lava", x, z)))
					return E_FAIL;
			}
		}

		/* 2. 뚫린 바닥 : 가운데 넓은 띠 (블럭이 놓인 칸 제외) */
		for (int z = iHoleTopZ; z <= iHoleBottomZ; ++z)
		{
			for (int x = 0; x <= iHoleEndX; ++x)
			{
				if (vecSolid[z * iSizeX + x]) continue;
				if (FAILED(SpawnEmptyTile(x, z)))
					return E_FAIL;
			}
		}

		/* 3. 뚫린 바닥 : 우측 플레이어 쪽으로 이어지는 얇은 띠 */
		for (int x = iHoleEndX + 1; x < iSizeX; ++x)
		{
			if (FAILED(SpawnEmptyTile(x, iStripZ)))
				return E_FAIL;
		}

		/* 4. 부서지는 블럭 'ㄱ' 3개 : 모양마다 첫 블럭을 헤드로 묶어 한 번에 무너짐 */
		for (const TBreakableShape& tShape : tShapes)
		{
			CCubeNetBreakable* pHead = nullptr;

			auto SpawnBreakable = [&](int iTileX, int iTileZ) -> HRESULT
				{
					CCubeNetBreakable* pBreakable = CCubeNetBreakable::Create(pDevice);
					if (nullptr == pBreakable) return E_FAIL;

					if (pHead) pBreakable->SetHead(pHead);
					else pHead = pBreakable;

					/* 무너지면 지형 목록에서 빼서 2D 플레이어가 죽은 블럭을 밟지 않게 함 */
					pBreakable->m_OnDead.AddBinding(GetToken(), [this](CCubeNetBreakable* p) { RemoveDeadTerrain(p); });

					return SpawnBlock(pBreakable, L"Breakable", iTileX, iTileZ);
				};

			for (int x = tShape.iStartX; x < tShape.iStartX + tShape.iWidth; ++x)
			{
				if (FAILED(SpawnBreakable(x, iBarZ)))
					return E_FAIL;
			}
			for (int z = iBarZ + 1; z <= iLegEndZ; ++z)
			{
				if (FAILED(SpawnBreakable(tShape.iStartX, z)))
					return E_FAIL;
			}
		}

		/* 5. 열쇠 블럭 기둥 : 천장 ~ 뚫린 띠 바로 위 */
		for (int z = 0; z < iHoleTopZ; ++z)
		{
			CCubeNetKeyBlock* pKeyBlock = CCubeNetKeyBlock::Create(pDevice);
			if (FAILED(SpawnBlock(pKeyBlock, L"KeyBlock", iKeyBlockX, z)))
				return E_FAIL;
			m_vecKeyBlock.push_back(pKeyBlock);
		}

		/* 6. 우측 잔디 발판 (2칸) */
		for (int x = iSizeX - 2; x < iSizeX; ++x)
		{
			if (FAILED(SpawnBlock(CCubeNetGrass::Create(pDevice), L"Grass", x, iBarZ)))
				return E_FAIL;
		}

		/* 7. 좌하단 나무 플랭크 */
		if (FAILED(SpawnBlock(CCubeNetPlank::Create(pDevice), L"Plank", 1, 18)))
			return E_FAIL;

		/* 8. 열쇠 : 가운데 아래 (먹으면 열쇠 블럭 파괴) */
		if (FAILED(SpawnBlock(COrthoKey::Create(pDevice), L"Key", iKeyBlockX, 18)))
			return E_FAIL;
		++m_iKeyCount;

		/* 9. 클리어 포탈 : 좌상단 */
		if (FAILED(SpawnBlock(COrthoPhaseGate::Create(pDevice), L"PhaseGate", 2, 7)))
			return E_FAIL;

		/* 시작 위치 : 2D 플레이어는 우측 잔디 발판 위, 3D 플레이어는 우하단 */
		const _vec3 vFlatTile = GetTileCenterPos(TTileIdx{ iBarZ - 1, iSizeX - 2 });
		vFlatPlayerPos = _vec3{ vFlatTile.x, 0.1f, vFlatTile.z };

		const _vec3 vPlayerTile = GetTileCenterPos(TTileIdx{ 18, iSizeX - 1 });
		vPlayerPos.x = vPlayerTile.x;
		vPlayerPos.z = vPlayerTile.z;
		break;
	}
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
	m_pFlatPlayer->GetMovement()->ResetVelocity();

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

EPlaceResult COrthoRoomLayer::RequestPlaceOrthoObject(CCubeNet* pObj, const _vec3& vHeldPos)
{
	if (nullptr == pObj) return EPlaceResult::BLOCKED;

	const TTileIdx tIdx = GetNearestTileIndex(vHeldPos);
	const int iIndex = ToFlatIndex(tIdx);

	/* 다른 블럭이 있는 칸에는 놓지 않음 */
	if (IsTileOccupied(iIndex, pObj))
		return EPlaceResult::BLOCKED;

	EPlaceResult eResult = EPlaceResult::PLACED;
	_vec3 vPos = GetTileCenterPos(tIdx);

	if (0 <= iIndex && iIndex < (int)m_vecEmptyTile.size() && m_vecEmptyTile[iIndex])
	{
		/* 뚫린 바닥 : 블럭이 구멍을 메운다 */
		vPos.y -= 0.5f * m_fMiniTileUnit;
		m_vecEmptyTile[iIndex] = false;
		eResult = EPlaceResult::FILLED_HOLE;

		/* 메운 칸은 바닥이 된 것 : 그 위에 다른 블럭을 놓을 수 있도록 점유하지 않음 */
		SetTileOccupant(-1, pObj);

		/* 메운 플랭크는 다시 들 수 없게 */
		if (CCubeNetPlank* pPlank = dynamic_cast<CCubeNetPlank*>(pObj))
			pPlank->LockInHole();

		/* 메운 블럭은 바닥 아래로 들어간 것 : 2D 플레이어와 상호작용하지 않도록 지형 목록에서 뺌 */
		if (IVerticalTerrain* pTerrain = dynamic_cast<IVerticalTerrain*>(pObj))
			RemoveDeadTerrain(pTerrain);

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
		SetTileOccupant(iIndex, pObj);
	}

	/* 소켓(로컬) 모드 해제 후 월드 위치로 */
	pObj->GetTransform()->SetUseLocal(false);
	pObj->GetTransform()->Set_Pos(vPos);

	return eResult;
}

bool COrthoRoomLayer::IsTileOccupied(int iIndex, CCubeNet* pIgnore) const
{
	if (iIndex < 0 || iIndex >= (int)m_vecTileOccupant.size()) return false;

	const TTileOccupant& tOccupant = m_vecTileOccupant[iIndex];
	return tOccupant.pObj && tOccupant.pObj != pIgnore && !tOccupant.wToken.expired();
}

void COrthoRoomLayer::SetTileOccupant(int iIndex, CCubeNet* pObj)
{
	if (nullptr == pObj) return;

	/* 원래 있던 칸 비우기 */
	for (TTileOccupant& tOccupant : m_vecTileOccupant)
	{
		if (tOccupant.pObj == pObj)
			tOccupant = TTileOccupant{};
	}

	if (iIndex < 0 || iIndex >= (int)m_vecTileOccupant.size()) return;

	m_vecTileOccupant[iIndex] = TTileOccupant{ pObj, pObj->GetToken() };
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
	/* 클리어 표시 (재생이 끝나면 UI가 스스로 꺼짐) */
	if (m_pPhaseClearUI)
		m_pPhaseClearUI->Play();

	CSoundMgr::GetInstance()->PlaySFX(L"seesaw_clear.mp3");

	/* 클리어 UI가 끝날 때까지 2D/3D 입력 잠금 (다음 페이즈 : 커튼이 다 걷힌 뒤 해제, 마지막 : EndOrthoGame에서 해제) */
	LockTransitionInput();

	/* 후속 처리는 클리어 UI 재생이 끝난 뒤 OnPhaseClearUIFinished에서 */
	m_eClearFollowUp = (m_iCurrentPhase >= m_iLastPhase) ? EClearFollowUp::END_GAME : EClearFollowUp::NEXT_PHASE;

	/* UI가 없으면(재생할 수 없으면) 바로 후속 처리 */
	if (nullptr == m_pPhaseClearUI || !m_pPhaseClearUI->IsPlaying())
		OnPhaseClearUIFinished();
}

void COrthoRoomLayer::OnPhaseClearUIFinished()
{
	const EClearFollowUp eFollowUp = m_eClearFollowUp;
	m_eClearFollowUp = EClearFollowUp::NONE;

	switch (eFollowUp)
	{
	case EClearFollowUp::NEXT_PHASE:
		/* 커튼으로 덮은 뒤 교체 (교체는 UpdateCurtain에서 AdvancePhase, 입력 해제는 커튼이 다 걷힌 뒤) */
		StartCurtain();
		break;
	case EClearFollowUp::END_GAME:
		FinishOrthoGame();
		break;
	default:
		break;
	}
}

void COrthoRoomLayer::FinishOrthoGame()
{
	/* 마지막 페이즈 : 페이즈 정리 -> 미니게임 종료 -> 방 클리어 요청 */
	CleanCurrentPhase();
	EndOrthoGame();
	/* 플레이어 카메라로 바로 돌아가지 않고 퇴장 연출 */
	StartExitDirecting();
}

void COrthoRoomLayer::StartOrthoGame()
{
	/* 게임기 : 입장 연출 동안 화면에 보여야 하므로 여기서 제거 */
	if (m_pGameMachine)
	{
		m_pGameMachine->Set_Dead(true);
		m_pGameMachine = nullptr;
	}

	/* 미니게임 중에는 카메라 방향과 상관없이 0도 기준으로 바라보고 이동 */
	m_pPlayer->SetYawFixedToZero(true);
	/* 미니게임 중에는 충돌체 크기와 수평 최대 속도를 절반으로 */
	m_pPlayer->SetMiniGameBodyCoef(0.5f);
	/* 미니게임 중에는 무기를 숨김 */
	if (CWeaponSystem* pWeaponSystem = m_pPlayer->GetWeaponSystem())
		pWeaponSystem->SetWeaponVisibility(false);
	/* 2D 플레이어는 시작할 때 켠다 */
	m_pFlatPlayer->Set_IsActive(true);
	/* 좌상단 페이즈 표시 */
	if (m_pPhaseNumberUI)
		m_pPhaseNumberUI->Show();

	/* 기존 타일을 끄고 0.5 미니 타일을 깜 : 이 시점부터 타일 유닛이 0.5 */
	if (FAILED(SpawnMiniTiles()))
		MSG_BOX("[COrthoRoomLayer] SpawnMiniTiles Failed");

	/* 페이즈 전환 커튼 블럭 : 미니 격자 기준으로 만들어만 두고 꺼둠 */
	if (FAILED(SpawnCurtainBlocks()))
		MSG_BOX("[COrthoRoomLayer] SpawnCurtainBlocks Failed");

	/* 벽지 : 미니게임용으로 교체 (종료 시 EndOrthoGame에서 복구) */
	SetMiniGameWallpaper(true);

	/* 직교 카메라 (입장 연출에서 이미 선택되어 있음) */
	CClientCameraMgr::GetInstance()->Select_Camera(CLIENT_CAMERA_TYPE::ORTHO);
	m_pOrthoCamera->UpdateView(GetCenterPos() + _vec3{ 0.f, 13.f, -4.f }, GetCenterPos());

	if (FAILED(SpawnPhase(m_iCurrentPhase)))
		MSG_BOX("[COrthoRoomLayer] SpawnPhase Failed");

	m_bPhaseStarted = true;

	/* 시작은 2D 플레이어 시점 : 원근 위치에서 직교 탑뷰로 전환 */
	SetGameMode(ORTHO);
}

void COrthoRoomLayer::SetMiniGameWallpaper(bool bOn)
{
	/* 방은 벽을 따로 들고 있지 않으므로 레이 테스트 목록에서 찾음 */
	for (IRayTestable* pTestable : m_vecRayTestable)
	{
		CWall* pWall = dynamic_cast<CWall*>(pTestable);
		if (nullptr == pWall)
			continue;

		if (!bOn)
		{
			pWall->ClearTextureOverride();
			continue;
		}

		if (FAILED(pWall->SetTextureOverride(L"Proto_ToyWall_Texture", 0)))
			MSG_BOX("[COrthoRoomLayer] Wall SetTextureOverride Failed");
	}
}

void COrthoRoomLayer::StartEnterDirecting()
{
	m_eDirecting = EDirecting::ENTERING;
	m_bEnterOrbitFinished = false;
	m_bIntroFinished = false;

	/* 입장 키를 누른 순간부터 3D 입력 잠금 (이동/상호작용은 게임이 시작될 때, 무기는 미니게임이 끝날 때 해제) */
	m_pPlayer->LockInput(EPlayerInputChannel::PIC_MOVE);
	m_pPlayer->LockInput(EPlayerInputChannel::PIC_WEAPON);
	m_pPlayer->GetMovement()->Stop();

	CLayer* pUILayer = CManagement::GetInstance()->Get_Layer(L"UI_Layer");
	if (pUILayer) pUILayer->Set_IsActive(false);

	/* 누른 순간의 플레이어 위치에서 출발해 게임기 화면 앞(I)까지, 화면(S)을 바라보며 직교 카메라로 다가감 */
	const _vec3 vCenter = GetCenterPos();
	const _vec3 vFloorCenter{ vCenter.x, 0.f, vCenter.z };

	_vec3 vPlayerPos;
	m_pPlayer->GetTransform()->Get_Info(INFO_POS, &vPlayerPos);

	CClientCameraMgr::GetInstance()->Select_Camera(CLIENT_CAMERA_TYPE::ORTHO);
	m_pOrthoCamera->Orbit(vPlayerPos + _vec3{0.f, 1.f, 0.f}, vFloorCenter + s_vEnterEye, vFloorCenter + s_vEnterAt, s_fEnterDuration,
		[this]() { m_bEnterOrbitFinished = true; });
}

void COrthoRoomLayer::StartExitDirecting()
{
	const _vec3 vCenter = GetCenterPos();
	const _vec3 vFloorCenter{ vCenter.x, 0.f, vCenter.z };

	/* 플레이어를 방 중앙 앞으로 옮기고 (높이는 그대로) 정면(+z)을 보게 함 */
	CTransform* pTransform = m_pPlayer->GetTransform();
	_vec3 vPlayerPos;
	pTransform->Get_Info(INFO_POS, &vPlayerPos);
	pTransform->Set_Pos(vFloorCenter.x + s_vExitPlayerPos.x, vPlayerPos.y, vFloorCenter.z + s_vExitPlayerPos.z);
	m_pPlayer->GetMovement()->Stop();
	m_pPlayer->SetYawFixedToZero(true);

	/* 연출이 끝나고 돌아갈 플레이어 카메라도 정면을 보도록 */
	if (CCamera* pPlayerCamera = CClientCameraMgr::GetInstance()->Find_Camera(CLIENT_CAMERA_TYPE::PLAYER))
		pPlayerCamera->Rotate(-pPlayerCamera->Get_Yaw(), -pPlayerCamera->Get_Pitch());

	/* 연출 동안 3D 이동/상호작용 잠금 */
	m_pPlayer->LockInput(EPlayerInputChannel::PIC_MOVE);
	m_eDirecting = EDirecting::EXITING;

	/* 시네 카메라 : 플레이어 뒤 위쪽에서 눈높이까지 직선으로 내려오며 +z 방향을 봄 */
	CCinematicCamera* pCinematic = dynamic_cast<CCinematicCamera*>(CClientCameraMgr::GetInstance()->Find_Camera(CLIENT_CAMERA_TYPE::CINEMATIC));
	if (nullptr == pCinematic)
	{
		FinishExitDirecting();
		return;
	}

	CINEMATIC_DESC tDesc;
	tDesc.vEyeFrom = vFloorCenter + s_vExitEyeFrom;
	tDesc.vEyeTo = vFloorCenter + s_vExitEyeTo;
	tDesc.vLookAt = tDesc.vEyeTo + _vec3{ 0.f, 0.f, 1.f };
	tDesc.fDuration = s_fExitDuration;

	/* 다른 연출에서 남은 설정이 섞이지 않도록 */
	pCinematic->Set_Target(nullptr);
	pCinematic->Set_StartFromCurrent(false);
	pCinematic->Add_Shot(tDesc);

	/* 재생이 끝나면 카메라 매니저가 플레이어 카메라로 되돌림 */
	if (FAILED(CClientCameraMgr::GetInstance()->Select_Camera(CLIENT_CAMERA_TYPE::CINEMATIC)))
	{
		CClientCameraMgr::GetInstance()->Select_Camera(CLIENT_CAMERA_TYPE::PLAYER);
		FinishExitDirecting();
	}

	CSoundMgr::GetInstance()->FadeOutBGM(s_fExitDuration);
}

void COrthoRoomLayer::FinishExitDirecting()
{
	m_eDirecting = EDirecting::NONE;

	m_pPlayer->SetYawFixedToZero(false);
	m_pPlayer->UnlockInput(EPlayerInputChannel::PIC_MOVE);

	CLayer* pUILayer = CManagement::GetInstance()->Get_Layer(L"UI_Layer");
	if (pUILayer) pUILayer->Set_IsActive(true);

	if (CWeaponSystem* pWeaponSystem = m_pPlayer->GetWeaponSystem())
		pWeaponSystem->SetWeaponVisibility(true);
	
	/* BGM : 미니게임 시작 전 트랙을 그 시점부터 이어서 재생 (원래 BGM이 없었으면 정지) */
	CSoundMgr::GetInstance()->RestoreBGM(m_tPrevBGM, 1.f);

	/* 방의 클리어 조건에 ManualClear가 있으면 이 요청으로 방이 클리어된다 */
	TRoomEventCtx t{ ERoomEventType::ROOM_MANUALCLEAR_REQUEST };
	m_OnRoomEvent.Broadcast(t);
}

void COrthoRoomLayer::UpdateDirecting(float fTimeDelta)
{
	switch (m_eDirecting)
	{
	case EDirecting::ENTERING:
		/* 화면 앞에 도착 : 인트로 재생 (재생할 수 없으면 바로 게임 시작) */
		if (m_bEnterOrbitFinished)
		{
			m_bEnterOrbitFinished = false;

			if (m_pIntroUI)
				m_pIntroUI->Play();
			if (nullptr == m_pIntroUI || !m_pIntroUI->IsPlaying())
				m_bIntroFinished = true;

			/* 원래 BGM 위치까지 백업 후 미니게임 BGM으로 교체 */
			m_tPrevBGM = CSoundMgr::GetInstance()->CaptureBGM();
			TBGMTrack tTrack = { TBGMTrack{L"Mini_Ortho", L"", L"The_Fantastic_Thinking_Newtro.wav"} };
			CSoundMgr::GetInstance()->PlayBGM(tTrack);
			CSoundMgr::GetInstance()->SetBGMVolume(0.3f);
		}

		/* 인트로가 아무 키로 끝나면 게임 시작 */
		if (m_bIntroFinished)
		{
			m_bIntroFinished = false;
			m_eDirecting = EDirecting::NONE;
			StartOrthoGame();
			/* 입장 연출 잠금 해제 (직교 시점 잠금은 SetGameMode에서 따로 걸려 있음) */
			m_pPlayer->UnlockInput(EPlayerInputChannel::PIC_MOVE);
		}
		break;
	case EDirecting::EXITING:
		/* 시네 카메라 재생이 끝나 플레이어 카메라로 돌아오면 종료 */
		if (CClientCameraMgr::GetInstance()->Get_ActiveCameraType() != CLIENT_CAMERA_TYPE::CINEMATIC)
			FinishExitDirecting();
		break;
	default:
		break;
	}
}

void COrthoRoomLayer::LockTransitionInput()
{
	if (m_bTransitionInputLocked) return;
	m_bTransitionInputLocked = true;

	/* 3D : 이동/상호작용(E) 잠금 (카운트 방식이라 직교 모드의 잠금과 겹쳐도 짝만 맞으면 됨) */
	m_pPlayer->LockInput(EPlayerInputChannel::PIC_MOVE);
	/* 걷던 중에 클리어되면 걷기 커맨드/애니메이션이 남으므로 아이들로 */
	m_pPlayer->ResetToIdle();
	/* 2D : 입력 끔 (걷던 커맨드도 함께 멈춤) */
	m_pFlatPlayer->SetInputEnabled(false);
}

void COrthoRoomLayer::UnlockTransitionInput()
{
	if (!m_bTransitionInputLocked) return;
	m_bTransitionInputLocked = false;

	m_pPlayer->UnlockInput(EPlayerInputChannel::PIC_MOVE);
	/* 2D 입력은 현재 시점 기준으로 복구 (직교일 때만 켜짐, SetGameMode와 동일) */
	m_pFlatPlayer->SetInputEnabled(m_eMode == ORTHO);
}

void COrthoRoomLayer::AdvancePhase()
{
	CleanCurrentPhase();
	++m_iCurrentPhase;

	if (FAILED(SpawnPhase(m_iCurrentPhase)))
		MSG_BOX("[COrthoRoomLayer] SpawnPhase Failed");
}

void COrthoRoomLayer::ResetCurrentPhase()
{
	/* 페이즈 오브젝트를 모두 지우고 같은 페이즈를 다시 스폰 : 위치/상태(열쇠, 부서진 블럭, 메운 구멍 등)가 전부 초기화 */
	CleanCurrentPhase();

	if (FAILED(SpawnPhase(m_iCurrentPhase)))
		MSG_BOX("[COrthoRoomLayer] SpawnPhase Failed");

	/* 맞았다는 표시 */
	m_pFlatPlayer->StartFlicker();
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
	fill(m_vecTileOccupant.begin(), m_vecTileOccupant.end(), TTileOccupant{});

	/* 게이트 : 죽은 게이트는 Exit가 오지 않으므로 직접 끔 */
	m_bPlayerOnGate = false;
	m_bFlatPlayerOnGate = false;
	m_bWasBothOnGate = false;
}

void COrthoRoomLayer::EndOrthoGame()
{
	if (!m_bPhaseStarted) return;
	m_bPhaseStarted = false;
	/* 종료 직전 프레임에 들어온 리셋 요청은 버림 */
	m_bPendingResetPhase = false;

	/* 전환 중 잠금이 남아 있으면 먼저 해제 (아래 입력 복구와 짝이 맞도록) */
	UnlockTransitionInput();

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
	m_pPlayer->SetYawFixedToZero(false);
	m_pPlayer->SetMiniGameBodyCoef(1.f);

	/* 타임스케일 복구 */
	CTimerMgr::GetInstance()->ClearGroupTimeScale(CTG_PLAYER);
	CTimerMgr::GetInstance()->ClearGroupTimeScale(CTG_FLATPLAYER);

	/* 2D 플레이어는 다시 꺼둠 */
	m_pFlatPlayer->SetInputEnabled(false);
	m_pFlatPlayer->Set_IsActive(false);

	/* 좌상단 페이즈 표시 숨김 */
	if (m_pPhaseNumberUI)
		m_pPhaseNumberUI->Hide();

	/* 마커 숨김 */
	m_bIsGrabbing = false;
	if (m_pMarker) m_pMarker->SetVisible(false);

	/* 전환 커튼 제거 */
	RemoveCurtainBlocks();

	/* 벽지 복구 */
	SetMiniGameWallpaper(false);

	/* 미니 타일 제거 + 기존 타일 복구 : 이 시점부터 타일 유닛이 다시 1 */
	RemoveMiniTiles();
}

void COrthoRoomLayer::SetGameMode(GAMEMODE eMode)
{
	if (m_eMode == eMode) return;

	if (eMode == ORTHO)
	{
		/* -> 직교로 변환 */
		m_pOrthoCamera->ToggleProjectionMode(0.5f);
		m_pOrthoCamera->Orbit(
			GetCenterPos() + _vec3{ 0.f, 13.f, -4.f },
			GetCenterPos() + _vec3{ 0.f, 13.f, 0.f },
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
			GetCenterPos() + _vec3{ 0.f, 13.f, 0.f },
			GetCenterPos() + _vec3{ 0.f, 13.f, -4.f },
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
	m_fMiniTileTime = 0.f;

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
			CTile* pOrigin = vecOriginByIndex[iOriginRow * iOriginSizeX + iOriginCol];
			if (pOrigin && pOrigin->GetType() == ETileType::SPRITE)
			{
				CSpriteTile* pOriginSprite = static_cast<CSpriteTile*>(pOrigin);
				iTextureIdx = pOriginSprite->GetTextureIdx();
			}

			const int iIndex = iRow * iSizeX + iCol;

			/* 배경 조각을 그리는 직교 전용 타일 (미니 격자 행/열과 칸 수를 넘김, 오염은 항상 저항) */
			COrthoSpriteTile* pMiniTile = COrthoSpriteTile::Create(pDevice, iIndex, iTextureIdx, TTileIdx{ iRow, iCol }, iSizeX, iSizeZ, &m_fMiniTileTime);
			if (nullptr == pMiniTile)
				return E_FAIL;

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
	m_vecTileOccupant.assign(iSizeX * iSizeZ, TTileOccupant{});

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
	m_vecTileOccupant.clear();
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

HRESULT COrthoRoomLayer::SpawnCurtainBlocks()
{
	LPDIRECT3DDEVICE9 pDevice = CGraphicDev::GetInstance()->Get_GraphicDev();

	/* 미니 격자 크기 (SpawnMiniTiles 이후 호출되므로 0.5 단위) */
	const _vec3 vInnerRoomSize = CRoomLoadingMgr::GetInstance()->GetInnerRoomSize();
	const float fUnit = GetTileUnit();
	m_iCurtainSizeX = (int)roundf(vInnerRoomSize.x / fUnit);
	m_iCurtainSizeZ = (int)roundf(vInnerRoomSize.z / fUnit);

	m_vecCurtainBlock.assign(m_iCurtainSizeX * m_iCurtainSizeZ, nullptr);

	for (int z = 0; z < m_iCurtainSizeZ; ++z)
	{
		for (int x = 0; x < m_iCurtainSizeX; ++x)
		{
			/* 테스트용 : 플랭크 블럭을 연출용으로만 사용 */
			CCubeNetPlank* pBlock = CCubeNetPlank::Create(pDevice);
			if (nullptr == pBlock) return E_FAIL;

			wstring wstrName = L"Room_" + to_wstring(m_iRoomIndex) + L"_Curtain_"
				+ to_wstring(z) + L"_" + to_wstring(x);
			if (FAILED(Add_GameObject(wstrName, pBlock)))
				return E_FAIL;

			const _vec3 vTile = GetTileCenterPos(TTileIdx{ z, x });
			pBlock->GetTransform()->Set_Pos(vTile.x, s_fCurtainHeight, vTile.z);
			pBlock->GetTransform()->Set_Scale(0.f, 0.f, 0.f);

			/* 연출 전용 : 콜라이더를 꺼서 충돌 판정에서 제외 (Add_Collider가 비활성 콜라이더는 받지 않음) */
			pBlock->LockInHole();
			pBlock->Set_IsActive(false);

			m_vecCurtainBlock[z * m_iCurtainSizeX + x] = pBlock;
		}
	}

	return S_OK;
}

void COrthoRoomLayer::RemoveCurtainBlocks()
{
	for (CCubeNetPlank* pBlock : m_vecCurtainBlock)
	{
		if (pBlock) pBlock->Set_Dead(true);
	}
	m_vecCurtainBlock.clear();
	m_iCurtainSizeX = 0;
	m_iCurtainSizeZ = 0;
	m_eCurtainState = ECurtainState::NONE;
	m_fCurtainTime = 0.f;
}

void COrthoRoomLayer::StartCurtain()
{
	/* 커튼이 없으면 연출 없이 바로 교체 */
	if (m_vecCurtainBlock.empty())
	{
		AdvancePhase();
		UnlockTransitionInput();
		return;
	}

	for (CCubeNetPlank* pBlock : m_vecCurtainBlock)
	{
		pBlock->GetTransform()->Set_Scale(0.f, 0.f, 0.f);
		pBlock->Set_IsActive(true);
	}

	m_eCurtainState = ECurtainState::CLOSING;
	m_fCurtainTime = 0.f;
}

void COrthoRoomLayer::UpdateCurtain(float fTimeDelta)
{
	m_fCurtainTime += fTimeDelta;

	auto EaseOutCubic = [](float t) { const float u = 1.f - t; return 1.f - u * u * u; };
	auto EaseInCubic = [](float t) { return t * t * t; };

	const bool bClosing = (m_eCurtainState == ECurtainState::CLOSING);
	const float fFullScale = m_fMiniTileUnit;	/* 블럭 하나가 칸 하나를 덮는 크기 */

	/* 합 = 행 + 열 : 좌상단(0, 0)이 0, 우하단이 최대 */
	const int iMaxSum = (m_iCurtainSizeX - 1) + (m_iCurtainSizeZ - 1);

	for (int iSum = 0; iSum <= iMaxSum; ++iSum)
	{
		/* 같은 합을 공유하는 블럭은 같은 시간축 */
		const float fLocal = (m_fCurtainTime - iSum * s_fCurtainStepDelay) / s_fCurtainScaleTime;
		const float t = max(0.f, min(fLocal, 1.f));
		const float fScale = bClosing
			? fFullScale * EaseOutCubic(t)			/* 덮기 : 빠르게 커졌다가 부드럽게 멈춤 */
			: fFullScale * (1.f - EaseInCubic(t));	/* 걷기 : 천천히 줄다가 빠르게 사라짐 */

		/* 직사각형이라 합만으로 만든 (행, 열)이 격자를 벗어날 수 있음
		   열 = 합 - 행 이 [0, SizeX - 1] 안에 있도록 행 범위를 먼저 자름
		   행 시작 = max(0, 합 - (SizeX - 1)), 행 끝 = min(SizeZ - 1, 합) */
		const int iRowBegin = max(0, iSum - (m_iCurtainSizeX - 1));
		const int iRowEnd = min(m_iCurtainSizeZ - 1, iSum);

		for (int iRow = iRowBegin; iRow <= iRowEnd; ++iRow)
		{
			const int iCol = iSum - iRow;
			CCubeNetPlank* pBlock = m_vecCurtainBlock[iRow * m_iCurtainSizeX + iCol];
			pBlock->GetTransform()->Set_Scale(fScale, fScale, fScale);
		}
	}

	/* 마지막 대각선까지 끝났는지 (덮기는 다 덮인 뒤 잠깐 대기) */
	const float fTotalTime = iMaxSum * s_fCurtainStepDelay + s_fCurtainScaleTime
		+ (bClosing ? s_fCurtainHoldTime : 0.f);
	if (m_fCurtainTime < fTotalTime)
		return;

	if (bClosing)
	{
		/* 다 덮인 상태에서 페이즈 교체 후 걷기 시작 */
		AdvancePhase();
		m_eCurtainState = ECurtainState::OPENING;
		m_fCurtainTime = 0.f;
	}
	else
	{
		/* 다 걷혔으면 꺼서 업데이트/렌더에서 빠지게 */
		for (CCubeNetPlank* pBlock : m_vecCurtainBlock)
			pBlock->Set_IsActive(false);

		m_eCurtainState = ECurtainState::NONE;
		m_fCurtainTime = 0.f;

		/* 다음 페이즈 시작 : 입력 복구 */
		UnlockTransitionInput();
	}
}

void COrthoRoomLayer::Free()
{
	CRoomLayer::Free();
}
