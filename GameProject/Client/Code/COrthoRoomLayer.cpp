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

	LPDIRECT3DDEVICE9 pDevice = CGraphicDev::GetInstance()->GetInstance()->Get_GraphicDev();
	m_pFlatPlayer = CFlatPlayer::Create(pDevice);
	if (!m_pFlatPlayer) return E_FAIL;
	Add_GameObject(L"Flat_Player", m_pFlatPlayer);

	return S_OK;
}

_int COrthoRoomLayer::Update_Layer(_float fTimeDelta)
{
	if (!IsValidUpdateTarget()) return S_OK;

	if (!m_bRegistered)
	{
		m_pFlatPlayer->GetTransform()->Set_Pos(GetCenterPos() + _vec3{ 5.f, 0.03f, 3.f }); /* z fighting 방지 위해 표면에서 조금 띄워둠 */
		m_pOrthoCamera = dynamic_cast<COrthoCamera*>(CClientCameraMgr::GetInstance()->Find_Camera(CLIENT_CAMERA_TYPE::ORTHO));
		CClientCameraMgr::GetInstance()->Select_Camera(CLIENT_CAMERA_TYPE::ORTHO);
		m_pOrthoCamera->UpdateView(GetCenterPos() + _vec3{ 0.f, 14.f, -6.f }, GetCenterPos());
		m_pPlayer = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene())->GetPlayer();

		m_bRegistered = true;
	}


	if (CDInputMgr::GetInstance()->Key_Down(DIK_K))
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
			m_pPlayer->LockInput(EPlayerInputChannel::PIC_WEAPON);
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
			m_pPlayer->UnlockInput(EPlayerInputChannel::PIC_WEAPON);
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

	const _vec3 vInnerRoomSize = CRoomLoadingMgr::GetInstance()->GetInnerRoomSize();
	const int iSizeX = (int)vInnerRoomSize.x;
	const int iSizeZ = (int)vInnerRoomSize.z;
	const _vec3 vCenter = GetCenterPos();

	/* 큐브 피벗이 중심이면 0.5f, 바닥이면 0.f */
	const float fBlockY = 0.5f;

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

			_vec3 vTileOffset{
				-(float)(iSizeX - 1) / 2.f + (float)iTileX,
				0.f,
				 (float)(iSizeZ - 1) / 2.f - (float)iTileZ
			};

			CTransform* pTransformCom = dynamic_cast<CTransform*>(pObj->Get_Component(ID_DYNAMIC, L"Com_Transform"));
			if (pTransformCom)
				pTransformCom->Set_Pos(vCenter.x + vTileOffset.x, fBlockY, vCenter.z + vTileOffset.z);

			// if (IRayTestable* p = dynamic_cast<IRayTestable*>(pObj)) m_vecRayTestable.push_back(p);
			if (IVerticalTerrain* p = dynamic_cast<IVerticalTerrain*>(pObj))         m_vecVerticalTerrain.push_back(p);

			return S_OK;
		};

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
		vKeyPos.y = 0.03f;
		pKey->GetTransform()->Set_Pos(vKeyPos);

		++m_iKeyCount;
	}

	/* 내려놓기 위치 마커 : 방마다 하나 */
	m_pMarker = COrthoMarker::Create(pDevice);
	if (FAILED(Add_GameObject(L"Room_" + to_wstring(m_iRoomIndex) + L"_Marker", m_pMarker)))
		return E_FAIL;

	/* 페이즈 게이트 */
	if (FAILED(SpawnBlock(COrthoPhaseGate::Create(pDevice), L"PhaseGate", 0, iSizeZ / 2)))
		return E_FAIL;

	return S_OK;
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
	cout << "clear " << m_iCurrentPhase << endl;
	++m_iCurrentPhase;
	/* 방의 클리어 조건에 ManualClear가 있으면 이 요청으로 방이 클리어된다 */
	//TRoomEventCtx t{ ERoomEventType::ROOM_MANUALCLEAR_REQUEST };
	//m_OnRoomEvent.Broadcast(t);
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
