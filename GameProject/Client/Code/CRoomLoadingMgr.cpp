#include "pch.h"
#include "CRoomLoadingMgr.h"
#include "CAbstractFactory.h"
#include "CManagement.h"
#include "CTile.h"
#include "CWall.h"
#include "CFog.h"
#include "CTransform.h"
#include "CGraphicDev.h"

IMPLEMENT_SINGLETON(CRoomLoadingMgr);

CRoomLoadingMgr::CRoomLoadingMgr()
{
	m_pFactory = new CAbstractFactory;
}

CRoomLoadingMgr::~CRoomLoadingMgr()
{
	Free();
}

void CRoomLoadingMgr::RegisterRoomData(int iIndex, const TRoomData& tData)
{
	m_mapRoomData.insert({ iIndex, tData });
}

HRESULT CRoomLoadingMgr::LoadRoomData(int iIndex)
{
	auto iter = m_mapRoomData.find(iIndex);

	if (iter == m_mapRoomData.end())
	{
		assert(0);
		return E_FAIL;
	}

	int iRoomRow = iIndex / m_iRoomColCount;
	int iRoomCol = iIndex % m_iRoomColCount;

	_vec3 vRoomOffset{
		-(float)(m_iRoomColCount - 1) / 2.f * m_vOuterRoomSize.x + m_vOuterRoomSize.x * (float)iRoomCol,
		0.f,
		(float)(m_iRoomRowCount - 1) / 2.f * m_vOuterRoomSize.z - m_vOuterRoomSize.z * (float)iRoomRow,
	};

	_vec3 vRoomCenterPos = _vec3{ m_vCenterRoomPosition.x + vRoomOffset.x, 0.f, m_vCenterRoomPosition.z + vRoomOffset.z };

	LPDIRECT3DDEVICE9 pDevice = CGraphicDev::GetInstance()->GetInstance()->Get_GraphicDev();
	TRoomData* pData = &iter->second;

	CGameObject* pGameObject = nullptr;
	CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");


	/* 타일 */
	for (size_t i = 0; i < pData->vecTile.size(); ++i)
	{
		int iTileX = (int)i % (int)m_vInnerRoomSize.x;
		int iTileZ = (int)i / (int)m_vInnerRoomSize.x;

		_vec3 vTileOffset{
			-(float)((int)m_vInnerRoomSize.x - 1) / 2.f * 1.f + 1.f * (float)iTileX,
			0.f,
			(float)((int)m_vInnerRoomSize.z - 1) / 2.f * 1.f - 1.f * (float)iTileZ
		};

		int iTileTextureIdx = (pData->vecTile.at(i) == 0) ? pData->iDefaultTileIdx : pData->vecTile.at(i);
		pGameObject = CTile::Create(pDevice, (int)i, iTileTextureIdx);
		if (nullptr == pGameObject)
			return E_FAIL;

		wstring wstrTileName = L"Room_" + to_wstring(iIndex) + L"_Tile_" + to_wstring(i);

		if (FAILED(pLayer->Add_GameObject(wstrTileName, pGameObject)))
			return E_FAIL;

		CTransform* pTransformCom = dynamic_cast<CTransform*>(
			pLayer->Get_Component(ID_DYNAMIC, wstrTileName, L"Com_Transform"));

		pTransformCom->Set_Pos(vRoomCenterPos.x + vTileOffset.x, 0.f, vRoomCenterPos.z + vTileOffset.z);
	}

	/* 벽 : 동남서북 순 */
	for (size_t i = 0; i < pData->vecDoorInfo.size(); ++i)
	{
		pGameObject = CWall::Create(pDevice, (EWallDir)(i + 1), pData->vecDoorInfo.at(i));
		if (nullptr == pGameObject)
			return E_FAIL;

		wstring wstrDoorName = L"Room_" + to_wstring(iIndex) + L"_Door_" + to_wstring(i);

		if (FAILED(pLayer->Add_GameObject(wstrDoorName, pGameObject)))
			return E_FAIL;

		CTransform* pTransformCom = dynamic_cast<CTransform*>(
			pLayer->Get_Component(ID_DYNAMIC, wstrDoorName, L"Com_Transform"));

		pTransformCom->Set_Pos(vRoomCenterPos.x, 0.f, vRoomCenterPos.z);

		/* 안개 */
		CWall* pWall = static_cast<CWall*>(pGameObject);
		if (pWall->HasDoor())
		{
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

				wstring wstrDoorName = L"Room_" + to_wstring(iIndex) + L"_Dir_" + to_wstring(iDir) + L"_Fog_" + to_wstring(i);

				if (FAILED(pLayer->Add_GameObject(wstrDoorName, pGameObject)))
					return E_FAIL;

				CTransform* pTransformCom = dynamic_cast<CTransform*>(
					pLayer->Get_Component(ID_DYNAMIC, wstrDoorName, L"Com_Transform"));

				pTransformCom->Set_Pos(vRoomCenterPos.x, 0.f, vRoomCenterPos.z);
				pTransformCom->Rotation(ROT_Y, 90.f * iDir);

				pTransformCom->Move_Pos(&vDir, 5.5f + (iDir % 2) * 1.f + 0.2f * i, 1.f);
			}

			/* 문 쪽 타일 */
			pGameObject = CTile::Create(pDevice, (int)i, (pData->vecDoorTile[iDir - 1] == 0) ? pData->iDefaultTileIdx : pData->vecDoorTile[iDir - 1]);
			if (nullptr == pGameObject)
				return E_FAIL;

			wstring wstrTileName = L"Room_" + to_wstring(iIndex) + L"_DoorTile_" + to_wstring(i);

			if (FAILED(pLayer->Add_GameObject(wstrTileName, pGameObject)))
				return E_FAIL;

			CTransform* pTransformCom = dynamic_cast<CTransform*>(
				pLayer->Get_Component(ID_DYNAMIC, wstrTileName, L"Com_Transform"));

			pTransformCom->Set_Pos(vRoomCenterPos.x, 0.f, vRoomCenterPos.z);
			pTransformCom->Move_Pos(&vDir, 6.f + (iDir % 2) * 1.f, 1.f);
		}


	}

	/* 맵 오브젝트 */
	for (size_t i = 0; i < pData->vecObjectTilingInfo.size(); ++i)
	{
		int iTileX = (int)i % (int)m_vInnerRoomSize.x;
		int iTileZ = (int)i / (int)m_vInnerRoomSize.x;

		_vec3 vTileOffset{
			-(float)((int)m_vInnerRoomSize.x - 1) / 2.f * 1.f + 1.f * (float)iTileX,
			0.f,
			(float)((int)m_vInnerRoomSize.z - 1) / 2.f * 1.f - 1.f * (float)iTileZ
		};

		int iType = pData->vecObjectTilingInfo.at(i);

		if (iType <= (int)EObjectType::NONE || iType >= (int)EObjectType::MAX)
		{
			continue;
		}

		pGameObject = m_pFactory->Create((EObjectType)iType);
		if (nullptr == pGameObject)
			return E_FAIL;

		wstring wstrTileName = L"Room_" + to_wstring(iIndex) + L"_ObjectTiling_" + to_wstring(i);

		if (FAILED(pLayer->Add_GameObject(wstrTileName, pGameObject)))
			return E_FAIL;

		CTransform* pTransformCom = dynamic_cast<CTransform*>(
			pLayer->Get_Component(ID_DYNAMIC, wstrTileName, L"Com_Transform"));

		pTransformCom->Set_Pos(vRoomCenterPos.x + vTileOffset.x, 0.f, vRoomCenterPos.z + vTileOffset.z);
	}

	for (auto& tMapEntity : pData->vecObjectInfo)
	{
		if (tMapEntity.iType <= (int)EObjectType::NONE || tMapEntity.iType >= (int)EObjectType::MAX)
		{
			continue;
		}

		pGameObject = m_pFactory->Create((EObjectType)tMapEntity.iType);
		if (nullptr == pGameObject)
			return E_FAIL;

		wstring wstrMonsterName = L"Room_" + to_wstring(iIndex) + L"_" + tMapEntity.wstrEntityName;

		if (FAILED(pLayer->Add_GameObject(wstrMonsterName, pGameObject)))
			return E_FAIL;

		CTransform* pTransformCom = dynamic_cast<CTransform*>(
			pLayer->Get_Component(ID_DYNAMIC, wstrMonsterName, L"Com_Transform"));

		pTransformCom->Set_Pos(vRoomCenterPos.x + tMapEntity.vPos.x, vRoomCenterPos.y + tMapEntity.vPos.y, vRoomCenterPos.z + tMapEntity.vPos.z);
	}

	return S_OK;
}

void CRoomLoadingMgr::Free()
{
	Safe_Release(m_pFactory);
}