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
	_int iExit = CLayer::Update_Layer(fTimeDelta);

	return S_OK;
}

void CRoomLayer::LateUpdate_Layer(const _float& fTimeDelta)
{
	CLayer::LateUpdate_Layer(fTimeDelta);
}

HRESULT CRoomLayer::SpawnRoom()
{
	TRoomData* t = CRoomLoadingMgr::GetInstance()->GetRoomData(m_iRoomIndex);
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
		pGameObject = CTile::Create(pDevice, (int)i, iTileTextureIdx);
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
	CGameStatusMgr::GetInstance()->UpdateCurrentRoomIndex(m_iRoomIndex);

	if (!m_bVisited)
	{
		CGameStatusMgr::GetInstance()->UpdateVisitTable(m_iRoomIndex);
		m_bVisited = true;
	}

	if (!m_bCleared)
	{
		m_OnRoomBegin.Broadcast();
	}
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