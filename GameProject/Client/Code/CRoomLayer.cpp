#include "pch.h"
#include "CRoomLayer.h"
#include "CRoomLoadingMgr.h"
#include "CTransform.h"
#include "Client_Struct.h"
#include "CGraphicDev.h"
#include "CWall.h"
#include "CFog.h"
#include "CTile.h"
#include "CTriggerBox.h"
#include "CAbstractFactory.h"
#include "CGameStatusMgr.h"
#include "CDoor.h"
#include "CKillAllEntityCondition.h"
#include "CManagement.h"

CRoomLayer::CRoomLayer(int iRoomIndex) : m_iRoomIndex(iRoomIndex)
{
}

CRoomLayer::~CRoomLayer()
{
}

HRESULT CRoomLayer::Ready_Layer()
{
	if (FAILED(CLayer::Ready_Layer()))
	{
		return E_FAIL;
	}

	return S_OK;
}

_int CRoomLayer::Update_Layer(const _float& fTimeDelta)
{
	// if (CGameStatusMgr::GetInstance()->GetCurrentRoomLayer() != this) return S_OK;

	_int iExit = CLayer::Update_Layer(fTimeDelta);

	/* 방문하지 않은 방이 바로 클리어 처리되는 것을 막기 위함 */
	if (!m_bCleared && m_bVisited)
	{
		CheckClearCondition();
	}

	PlayerTileInteraction();

	return S_OK;
}

void CRoomLayer::PlayerTileInteraction()
{
	CTransform* pTransform = static_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
	_vec3 vPos;
	pTransform->Get_Info(INFO_POS, &vPos);
	
	CTile* pTile = GetTileFromWorldPosition(vPos);
	if (!pTile) return;
	
	EContaminateType eType = pTile->GetContaminationType();
	switch (eType)
	{
	case EContaminateType::LAVA:
		/* TODO 플레이어 데미지 */
		break;
	default:
		break;
	}
}

void CRoomLayer::LateUpdate_Layer(const _float& fTimeDelta)
{
	// if (CGameStatusMgr::GetInstance()->GetCurrentRoomLayer() != this) return;

	CLayer::LateUpdate_Layer(fTimeDelta);
}

HRESULT CRoomLayer::SpawnRoom()
{
	TRoomData* t = CRoomLoadingMgr::GetInstance()->GetRoomData(m_iRoomIndex);

	/* 클리어 조건 */
	for (auto& wstrClearCondtiion : t->vecClearCondition)
	{
		CClearCondition* pCondition = nullptr;

		if (wstrClearCondtiion == L"KillAllEntity")
		{
			pCondition = CKillAllEntityCondition::Create(this);
		}

		if (nullptr == pCondition)
		{
			assert(0);
			continue;
		}

		m_vecClearCondition.push_back(pCondition);
	}

	/* 진입 시 어둠 여부 */
	m_bDark= t->bDark;

	/* 방 기본 정보 */
	int iRoomColCount = CRoomLoadingMgr::GetInstance()->GetRoomColCount();
	int iRoomRowCount = CRoomLoadingMgr::GetInstance()->GetRoomRowCount();
	_vec3 vOuterRoomSize = CRoomLoadingMgr::GetInstance()->GetOuterRoomSize();
	_vec3 vInnerRoomSize = CRoomLoadingMgr::GetInstance()->GetInnerRoomSize();
	_vec3 vCenterRoomPosition = CRoomLoadingMgr::GetInstance()->GetCenterRoomPosition();

	int iRoomRow = m_iRoomIndex / iRoomColCount;
	int iRoomCol = m_iRoomIndex % iRoomColCount;

	_vec3 vRoomOffset{
		-(float)(iRoomColCount - 1) / 2.f * vOuterRoomSize.x + vOuterRoomSize.x * (float)iRoomCol,
		0.f,
		(float)(iRoomRowCount - 1) / 2.f * vOuterRoomSize.z - vOuterRoomSize.z * (float)iRoomRow,
	};

	_vec3 vRoomCenterPos = _vec3{ vCenterRoomPosition.x + vRoomOffset.x, 0.f, vCenterRoomPosition.z + vRoomOffset.z };
	m_vRoomCenterPos = vRoomCenterPos;

	LPDIRECT3DDEVICE9 pDevice = CGraphicDev::GetInstance()->GetInstance()->Get_GraphicDev();

	CGameObject* pGameObject = nullptr;

	/* 타일 */
	for (size_t i = 0; i < t->vecTile.size(); ++i)
	{
		int iTileX = (int)i % (int)vInnerRoomSize.x;
		int iTileZ = (int)i / (int)vInnerRoomSize.x;

		_vec3 vTileOffset{
			-(float)((int)vInnerRoomSize.x - 1) / 2.f * 1.f + 1.f * (float)iTileX,
			0.f,
			(float)((int)vInnerRoomSize.z - 1) / 2.f * 1.f - 1.f * (float)iTileZ
		};

		int iTileTextureIdx = (t->vecTile.at(i) == 0) ? t->iDefaultTileIdx : t->vecTile.at(i);
		bool bResistContamination = t->vecResistContamination.at(i) == 1;
		pGameObject = CTile::Create(pDevice, (int)i, iTileTextureIdx, bResistContamination);
		if (nullptr == pGameObject)
			return E_FAIL;

		wstring wstrTileName = L"Room_" + to_wstring(m_iRoomIndex) + L"_Tile_" + to_wstring(i);

		if (FAILED(Add_GameObject(wstrTileName, pGameObject)))
			return E_FAIL;

		CTransform* pTransformCom = dynamic_cast<CTransform*>(Get_Component(ID_DYNAMIC, wstrTileName, L"Com_Transform"));

		pTransformCom->Set_Pos(vRoomCenterPos.x + vTileOffset.x, 0.f, vRoomCenterPos.z + vTileOffset.z);
	}

	/* 벽 : 동남서북 순 */
	for (size_t i = 0; i < t->vecDoorInfo.size(); ++i)
	{
		pGameObject = CWall::Create(pDevice, (EWallDir)(i + 1), t->vecDoorInfo.at(i));
		if (nullptr == pGameObject)
			return E_FAIL;

		wstring wstrWallName = L"Room_" + to_wstring(m_iRoomIndex) + L"_Wall_" + to_wstring(i);

		if (FAILED(Add_GameObject(wstrWallName, pGameObject)))
			return E_FAIL;

		CTransform* pTransformCom = dynamic_cast<CTransform*>(Get_Component(ID_DYNAMIC, wstrWallName, L"Com_Transform"));

		pTransformCom->Set_Pos(vRoomCenterPos.x, 0.f, vRoomCenterPos.z);

		CWall* pWall = static_cast<CWall*>(pGameObject);
		if (pWall->HasDoor())
		{
			/* 안개 */
			int iDir = (int)pWall->GetDir();

			_vec3 vDir{ 0.f, 0.f, 1.f };
			_matrix matRot;
			D3DXMatrixRotationY(&matRot, D3DXToRadian(90.f) * iDir);
			D3DXVec3TransformCoord(&vDir, &vDir, &matRot);

			for (int i = 0; i < 5; i++)
			{
				pGameObject = CFog::Create(pDevice);
				if (nullptr == pGameObject)
					return E_FAIL;

				wstring wstrDoorName = L"Room_" + to_wstring(m_iRoomIndex) + L"_Dir_" + to_wstring(iDir) + L"_Fog_" + to_wstring(i);

				if (FAILED(Add_GameObject(wstrDoorName, pGameObject)))
					return E_FAIL;

				CTransform* pTransformCom = dynamic_cast<CTransform*>(Get_Component(ID_DYNAMIC, wstrDoorName, L"Com_Transform"));

				pTransformCom->Set_Pos(vRoomCenterPos.x, 0.f, vRoomCenterPos.z);
				pTransformCom->Rotation(ROT_Y, 90.f * iDir);

				pTransformCom->Move_Pos(&vDir, 5.5f + (iDir % 2) * 1.f + 0.2f * i, 1.f);
			}

			/* 문 쪽 타일 */
			pGameObject = CTile::Create(pDevice, (int)i, (t->vecDoorTile[iDir - 1] == 0) ? t->iDefaultTileIdx : t->vecDoorTile[iDir - 1]);
			if (nullptr == pGameObject)
				return E_FAIL;

			wstring wstrTileName = L"Room_" + to_wstring(m_iRoomIndex) + L"_DoorTile_" + to_wstring(i);

			if (FAILED(Add_GameObject(wstrTileName, pGameObject)))
				return E_FAIL;

			CTransform* pTransformCom = dynamic_cast<CTransform*>(Get_Component(ID_DYNAMIC, wstrTileName, L"Com_Transform"));

			pTransformCom->Set_Pos(vRoomCenterPos.x, 0.f, vRoomCenterPos.z);
			pTransformCom->Move_Pos(&vDir, 6.f + (iDir % 2) * 1.f, 1.f);

			/* 시작 트리거 박스 */
			pGameObject = CTriggerBox::Create(pDevice);
			if (nullptr == pGameObject)
				return E_FAIL;

			wstring wstrBoxName = L"Room_" + to_wstring(m_iRoomIndex) + L"_TriggerBox_" + to_wstring(i);

			if (FAILED(Add_GameObject(wstrBoxName, pGameObject)))
				return E_FAIL;

			pTransformCom = dynamic_cast<CTransform*>(Get_Component(ID_DYNAMIC, wstrBoxName, L"Com_Transform"));

			pTransformCom->Set_Pos(vRoomCenterPos.x, 0.f, vRoomCenterPos.z);
			pTransformCom->Move_Pos(&vDir, 4.f + (iDir % 2) * 1.f, 1.f);

			/* 문 */
			
			pGameObject = CDoor::Create(pDevice);
			if (nullptr == pGameObject)
				return E_FAIL;

			wstring wstrDoorName = L"Room_" + to_wstring(m_iRoomIndex) + L"_Door_" + to_wstring(i);

			if (FAILED(Add_GameObject(wstrDoorName, pGameObject)))
				return E_FAIL;

			pTransformCom = dynamic_cast<CTransform*>(Get_Component(ID_DYNAMIC, wstrDoorName, L"Com_Transform"));

			pTransformCom->Set_Pos(vRoomCenterPos.x, 0.75f, vRoomCenterPos.z);
			pTransformCom->Rotation(ROT_Y, 90.f * iDir);
			pTransformCom->Move_Pos(&vDir, 5.5f + (iDir % 2) * 1.f, 1.f);

		}


	}

	/* 맵 오브젝트 */
	for (size_t i = 0; i < t->vecObjectTilingInfo.size(); ++i)
	{
		int iTileX = (int)i % (int)vInnerRoomSize.x;
		int iTileZ = (int)i / (int)vInnerRoomSize.x;

		_vec3 vTileOffset{
			-(float)((int)vInnerRoomSize.x - 1) / 2.f * 1.f + 1.f * (float)iTileX,
			0.f,
			(float)((int)vInnerRoomSize.z - 1) / 2.f * 1.f - 1.f * (float)iTileZ
		};

		int iType = t->vecObjectTilingInfo.at(i);

		if (iType <= (int)EObjectType::NONE || iType >= (int)EObjectType::MAX)
		{
			continue;
		}

		pGameObject = CAbstractFactory::GetInstance()->Create((EObjectType)iType);
		if (nullptr == pGameObject)
			return E_FAIL;

		wstring wstrTileName = L"Room_" + to_wstring(m_iRoomIndex) + L"_ObjectTiling_" + to_wstring(i);

		if (FAILED(Add_GameObject(wstrTileName, pGameObject)))
			return E_FAIL;

		CTransform* pTransformCom = dynamic_cast<CTransform*>(Get_Component(ID_DYNAMIC, wstrTileName, L"Com_Transform"));

		pTransformCom->Set_Pos(vRoomCenterPos.x + vTileOffset.x, 0.f, vRoomCenterPos.z + vTileOffset.z);
	}

	for (auto& tMapEntity : t->vecObjectInfo)
	{
		if (tMapEntity.iType <= (int)EObjectType::NONE || tMapEntity.iType >= (int)EObjectType::MAX)
		{
			continue;
		}

		pGameObject = CAbstractFactory::GetInstance()->Create((EObjectType)tMapEntity.iType);
		if (nullptr == pGameObject)
			return E_FAIL;

		wstring wstrMonsterName = L"Room_" + to_wstring(m_iRoomIndex) + L"_" + tMapEntity.wstrEntityName;

		if (FAILED(Add_GameObject(wstrMonsterName, pGameObject)))
			return E_FAIL;

		CTransform* pTransformCom = dynamic_cast<CTransform*>(Get_Component(ID_DYNAMIC, wstrMonsterName, L"Com_Transform"));

		pTransformCom->Set_Pos(vRoomCenterPos.x + tMapEntity.vPos.x, vRoomCenterPos.y + tMapEntity.vPos.y, vRoomCenterPos.z + tMapEntity.vPos.z);
	}

	return S_OK;

}

void CRoomLayer::OnRoomTriggerBlockCollided()
{
	if (m_bOnProgress) return;	

	if (!m_bVisited)
	{
		CGameStatusMgr::GetInstance()->UpdateVisitTable(m_iRoomIndex);
		m_bVisited = true;
	}

	if (!m_bCleared)
	{
		TRoomEventCtx t{ ERoomEventType::ROOM_BEGIN };
		m_OnRoomEvent.Broadcast(t);
		m_bOnProgress = true;
	}
}

void CRoomLayer::RequestTileContamination(const _vec3& vPos, int iRange, EContaminateType eType, float fDuration)
{
	if (iRange <= 0) return;

	CTile* pCenterTile = GetTileFromWorldPosition(vPos);
	if (!pCenterTile) return;

	TTileIdx tCenterIdx = pCenterTile->GetPositionIndex2D();

	/* L1 Norm */
	for (int dz = -(iRange - 1); dz <= (iRange - 1); ++dz)
	{
		const int iRemain = (iRange - 1) - abs(dz);

		for (int dx = -iRemain; dx <= iRemain; ++dx)
		{
			CTile* pTile = GetTileFromIndex2D(TTileIdx{ tCenterIdx.iRow + dz, tCenterIdx.iCol + dx });
			if (nullptr == pTile) continue;

			pTile->Contaminate(eType, fDuration);
		}
	}
}

void CRoomLayer::SetPseudoDark(bool bFlag)
{
	for (int i = 0; i < 4; ++i)
	{
		wstring wstrName = L"PseudoDark_" + to_wstring(i);
		CGameObject* pDark = CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", wstrName.c_str());
		pDark->Set_IsActive(bFlag);
	}
}

void CRoomLayer::CheckClearCondition()
{
	for (auto& c : m_vecClearCondition)
	{
		if (!c->IsSatisfied()) return;
	}

	/* 모든 클리어 조건이 만족 */
	CGameStatusMgr::GetInstance()->UpdateClearTable(m_iRoomIndex);
	TRoomEventCtx t{ ERoomEventType::ROOM_CLEAR };
	m_OnRoomEvent.Broadcast(t);
	m_bOnProgress = false;
	m_bCleared = true;
}

CTile* CRoomLayer::GetTileFromWorldPosition(const _vec3& vWorldPos)
{
	_vec3 vInnerRoomSize = CRoomLoadingMgr::GetInstance()->GetInnerRoomSize();

	const int iSizeX = (int)vInnerRoomSize.x;
	const int iSizeZ = (int)vInnerRoomSize.z;

	if (0 >= iSizeX || 0 >= iSizeZ) return nullptr;

	const float fLocalX = vWorldPos.x - m_vRoomCenterPos.x;
	const float fLocalZ = vWorldPos.z - m_vRoomCenterPos.z;

	const int iTileX = (int)floorf(fLocalX + (float)iSizeX * 0.5f);
	const int iTileZ = (int)floorf((float)iSizeZ * 0.5f - fLocalZ);

	if (0 > iTileX || iTileX >= iSizeX || 0 > iTileZ || iTileZ >= iSizeZ) return nullptr;

	const size_t iIndex = (size_t)iTileZ * (size_t)iSizeX + (size_t)iTileX;
	if (iIndex >= iSizeX * iSizeZ) return nullptr;

	wstring wstrTileName = L"Room_" + to_wstring(m_iRoomIndex) + L"_Tile_" + to_wstring(iIndex);

	return static_cast<CTile*>(Get_GameObject(wstrTileName));
}

void CRoomLayer::ApplyDarkness()
{
	SetPseudoDark(m_bDark);
}

CTile* CRoomLayer::GetTileFromIndex2D(const TTileIdx& tIdx)
{
	const _vec3 vSize = CRoomLoadingMgr::GetInstance()->GetInnerRoomSize();
	const int iSizeX = (int)vSize.x;
	const int iSizeZ = (int)vSize.z;

	if (0 > tIdx.iCol || tIdx.iCol >= iSizeX || 0 > tIdx.iRow || tIdx.iRow >= iSizeZ) return nullptr;

	const int iIndex = tIdx.iRow * iSizeX + tIdx.iCol;

	wstring wstrTileName = L"Room_" + to_wstring(m_iRoomIndex) + L"_Tile_" + to_wstring(iIndex);

	return static_cast<CTile*>(Get_GameObject(wstrTileName));
}

CRoomLayer* CRoomLayer::Create(int iRoomIndex)
{
	CRoomLayer* pLayer = new CRoomLayer(iRoomIndex);

	if (FAILED(pLayer->Ready_Layer()))
	{
		Safe_Release(pLayer);
		MSG_BOX("[CRoomLayer] Layer Create Failed");
		return nullptr;
	}

	return pLayer;
}

void CRoomLayer::Free()
{
	for (auto& c : m_vecClearCondition)
	{
		Safe_Release(c);
	}

	CLayer::Free();
}
