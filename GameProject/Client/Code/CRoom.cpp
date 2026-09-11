#include "pch.h"
#include "CRoom.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CTile.h"
#include "CWall.h"
#include "CFog.h"
#include "CManagement.h"
#include "CRoomLoadingMgr.h"
#include "CLayer.h"
#include "CMonster.h"

CRoom::CRoom(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CRoom::CRoom(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex)
    : CGameObject(pGraphicDev), m_iIndex(iIndex)
{
}

CRoom::~CRoom()
{
}

HRESULT CRoom::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (m_iIndex == -1)
    {
        assert(0);
        return E_FAIL;
    }

    UpdateCenterPosition();

	TRoomData* pData = CRoomLoadingMgr::GetInstance()->GetRoomData(m_iIndex);
	const int iRoomTotalCount = CRoomLoadingMgr::GetInstance()->GetRoomTotalCount();
	const int iRoomRowCount = CRoomLoadingMgr::GetInstance()->GetRoomRowCount();
	const int iRoomColCount = CRoomLoadingMgr::GetInstance()->GetRoomColCount();
	const _vec3 vRoomTileSize = CRoomLoadingMgr::GetInstance()->GetInnerRoomSize();
	const _vec3 vRoomInterval = CRoomLoadingMgr::GetInstance()->GetOuterRoomSize();

	CGameObject* pGameObject = nullptr;
	CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");

	int iRoomRow = m_iIndex / iRoomColCount;
	int iRoomCol = m_iIndex % iRoomColCount;
		
	_vec3 vRoomOffset{
		-(float)(iRoomRowCount - 1) / 2.f * vRoomInterval.x + vRoomInterval.x * (float)iRoomRow,
		0.f,
		(float)(iRoomColCount - 1) / 2.f * vRoomInterval.z - vRoomInterval.z * (float)iRoomCol
	};


	/* 타일 */
	for (size_t i = 0; i < pData->vecTile.size(); ++i)
	{
		int iTileX = (int)i % (int)vRoomTileSize.x;
		int iTileZ = (int)i / (int)vRoomTileSize.x;

		_vec3 vTileOffset{
			-(float)((int)vRoomTileSize.x - 1) / 2.f * 1.f + 1.f * (float)iTileX,
			0.f,
			(float)((int)vRoomTileSize.z - 1) / 2.f * 1.f - 1.f * (float)iTileZ
		};

		int iTileTextureIdx = (pData->vecTile.at(i) == 0) ? pData->iDefaultTileIdx : pData->vecTile.at(i);
		pGameObject = CTile::Create(m_pGraphicDev, (int)i, iTileTextureIdx);
		if (nullptr == pGameObject)
			return E_FAIL;

		wstring wstrTileName = L"Room_" + to_wstring(m_iIndex) + L"_Tile_" + to_wstring(i);

		if (FAILED(pLayer->Add_GameObject(wstrTileName, pGameObject)))
			return E_FAIL;

		CTransform* pTransformCom = dynamic_cast<CTransform*>(
			pLayer->Get_Component(ID_DYNAMIC, wstrTileName, L"Com_Transform"));

		pTransformCom->Set_Pos(m_vCenterPos.x + vRoomOffset.x + vTileOffset.x, 0.f, m_vCenterPos.z + vRoomOffset.z + vTileOffset.z);
	}

	/* 벽 : 동남서북 순 */
	for (size_t i = 0; i < pData->vecDoorInfo.size(); ++i)
	{
		pGameObject = CWall::Create(m_pGraphicDev, (EWallDir)(i + 1), pData->vecDoorInfo.at(i));
		if (nullptr == pGameObject)
			return E_FAIL;

		wstring wstrDoorName = L"Room_" + to_wstring(m_iIndex) + L"_Door_" + to_wstring(i);

		if (FAILED(pLayer->Add_GameObject(wstrDoorName, pGameObject)))
			return E_FAIL;

		CTransform* pTransformCom = dynamic_cast<CTransform*>(
			pLayer->Get_Component(ID_DYNAMIC, wstrDoorName, L"Com_Transform"));

		pTransformCom->Set_Pos(m_vCenterPos.x + vRoomOffset.x, 0.f, m_vCenterPos.z + vRoomOffset.z);

		/* 안개 */
		CWall* pWall = static_cast<CWall*>(pGameObject);
		if (pWall->HasDoor())
		{
			int iDir = (int)pWall->GetDir();
			for (int i = 0; i < 5; i++)
			{
				pGameObject = CFog::Create(m_pGraphicDev);
				if (nullptr == pGameObject)
					return E_FAIL;

				wstring wstrDoorName = L"Room_" + to_wstring(m_iIndex) + L"_Dir_" + to_wstring(iDir) + L"_Fog_" + to_wstring(i);

				if (FAILED(pLayer->Add_GameObject(wstrDoorName, pGameObject)))
					return E_FAIL;

				CTransform* pTransformCom = dynamic_cast<CTransform*>(
					pLayer->Get_Component(ID_DYNAMIC, wstrDoorName, L"Com_Transform"));

				pTransformCom->Set_Pos(m_vCenterPos.x + vRoomOffset.x, 0.f, m_vCenterPos.z + vRoomOffset.z);
				pTransformCom->Rotation(ROT_Y, 90.f * iDir);

				_vec3 vDir{ 0.f, 0.f, 1.f };
				_matrix matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(90.f) * iDir);
				D3DXVec3TransformCoord(&vDir, &vDir, &matRot);
				pTransformCom->Move_Pos(&vDir, 5.5f + (iDir % 2) * 1.f + 0.2f * i, 1.f);
			}
		}
	}

	/* 몬스터 소환 */
	for (auto& tMapEntity : pData->vecMonsterInfo)
	{
		pGameObject = CMonster::Create(m_pGraphicDev);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(pLayer->Add_GameObject(tMapEntity.wstrEntityName, pGameObject)))
			return E_FAIL;

		CTransform* pTransformCom = dynamic_cast<CTransform*>(
			pLayer->Get_Component(ID_DYNAMIC, tMapEntity.wstrEntityName, L"Com_Transform"));

		pTransformCom->Set_Pos(m_vCenterPos.x + vRoomOffset.x + tMapEntity.vPos.x, m_vCenterPos.y + vRoomOffset.y + tMapEntity.vPos.y, m_vCenterPos.z + vRoomOffset.z + tMapEntity.vPos.z);
	}

    return S_OK;
}

void CRoom::UpdateCenterPosition()
{
    _vec3 vTerrainOffset{ 60.f, 0.f, 60.f };

    const int iGridSize = 5;
    _vec2 vMapSize{ 15.f, 13.f };

    int iRoomRow = m_iIndex / iGridSize;
    int iRoomCol = m_iIndex % iGridSize;

    _vec3 vRoomOffset{
        -(float)(iGridSize - 1) / 2.f * vMapSize.x + vMapSize.x * (float)iRoomRow,
        0.f,
        (float)(iGridSize - 1) / 2.f * vMapSize.y - vMapSize.y * (float)iRoomCol,
    };

    m_vCenterPos = _vec3{ vTerrainOffset.x + vRoomOffset.x, 0.f, vTerrainOffset.z + vRoomOffset.z };
    m_pTransformCom->Set_Pos(m_vCenterPos);
}

_int CRoom::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    return iExit;
}

void CRoom::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CRoom::Render_GameObject()
{
}

HRESULT CRoom::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    return S_OK;
}

CRoom* CRoom::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex)
{
    CRoom* pRoom = new CRoom(pGraphicDev, iIndex);

    if (FAILED(pRoom->Ready_GameObject()))
    {
        Safe_Release(pRoom);
        MSG_BOX("CRoom Create Failed");
        return nullptr;
    }

    return pRoom;
}

void CRoom::Free()
{
    CGameObject::Free();
}
