#include "pch.h"
#include "CStage.h"
#include "CBackGround.h"
#include "CProtoMgr.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CTerrain.h"
#include "CDynamicCamera.h"
#include "CSkyBox.h"
#include "CLightMgr.h"
#include "CEffect.h"
#include "CGun.h"
#include "CLoading.h"
#include "CManagement.h"
#include "CTile.h"
#include "CWall.h"
#include "CCollisionMgr.h"

CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
}

CStage::~CStage()
{
}

HRESULT CStage::Ready_Scene()
{
	CLoading::ParseMapData(&m_MapData);

	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
		return E_FAIL;

	if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(L"UI_Layer")))
		return E_FAIL;

	// 충돌 그룹 설정
	Engine::CCollisionMgr::GetInstance()->Check_Group(Engine::COLL_PLAYER, Engine::COLL_MONSTER);

	return S_OK;
}

_int CStage::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = CScene::Update_Scene(fTimeDelta);

	return iExit;
}

void CStage::LateUpdate_Scene(const _float& fTimeDelta)
{
	CScene::LateUpdate_Scene(fTimeDelta);

	Engine::CCollisionMgr::GetInstance()->Update_Collision();
	Engine::CCollisionMgr::GetInstance()->Clear_ColliderList();
}

void CStage::Render_Scene()
{

}

HRESULT CStage::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	// 오브젝트 추가
	CGameObject* pGameObject = nullptr;

	// DynamicCamera
	_vec3   vEye{ 0.f, 10.f, -10.f };
	_vec3   vAt{ 0.f, 0.f, 1.f };
	_vec3   vUp{ 0.f, 1.f, 0.f };

	pGameObject = CDynamicCamera::Create(m_pGraphicDev, 
										&vEye, &vAt,&vUp);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"DynamicCamera", pGameObject)))
		return E_FAIL;

	// SkyBox
	pGameObject = CSkyBox::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"SkyBox", pGameObject)))
		return E_FAIL;

	/* 하드코딩으로 5 x 5 격자 맵 */
	int iGridSize = 5;
	int iTileSizeX = 13;
	int iTileSizeZ = 11;
	_vec2 vMapSize{ 15.f, 13.f };
	_vec3 vTerrainOffset{ 60.f, 0.f, 60.f };

	for (int j = 0; j < iGridSize; ++j)
	{
		for (int k = 0; k < iGridSize; ++k)
		{
			int iRoomNumber = j * iGridSize + k;
			_vec3 vRoomOffset{
				-(float)(iGridSize - 1) / 2.f * vMapSize.x + vMapSize.x * (float)k,
				0.f,
				(float)(iGridSize - 1) / 2.f * vMapSize.y - vMapSize.y * (float)j
			};

			/* 타일 */
			for (size_t i = 0; i < m_MapData.vecTile.size(); ++i)
			{
				int iTileX = (int)i % iTileSizeX;
				int iTileZ = (int)i / iTileSizeX;

				_vec3 vTileOffset{
					-(float)(iTileSizeX - 1) / 2.f * 1.f + 1.f * (float)iTileX,
					0.f,
					(float)(iTileSizeZ - 1) / 2.f * 1.f - 1.f * (float)iTileZ
				};

				pGameObject = CTile::Create(m_pGraphicDev, (int)i, m_MapData.vecTile.at(i));
				if (nullptr == pGameObject)
					return E_FAIL;

				wstring wstrTileName = L"Room_" + to_wstring(iRoomNumber) + L"_Tile_" + to_wstring(i);

				if (FAILED(pLayer->Add_GameObject(wstrTileName, pGameObject)))
					return E_FAIL;

				CTransform* pTransformCom = dynamic_cast<CTransform*>(
					pLayer->Get_Component(ID_DYNAMIC, wstrTileName, L"Com_Transform"));

				pTransformCom->Set_Pos(vRoomOffset.x + vTileOffset.x + vTerrainOffset.x, 0.f, vRoomOffset.z + vTileOffset.z + vTerrainOffset.z);
			}

			/* 벽 : 동서남북 순 */
			for (size_t i = 0; i < m_MapData.vecDoorInfo.size(); ++i)
			{
				pGameObject = CWall::Create(m_pGraphicDev, (EWallDir)(i + 1), m_MapData.vecDoorInfo.at(i));
				if (nullptr == pGameObject)
					return E_FAIL;

				wstring wstrDoorName = L"Room_" + to_wstring(iRoomNumber) + L"_Door_" + to_wstring(i);

				if (FAILED(pLayer->Add_GameObject(wstrDoorName, pGameObject)))
					return E_FAIL;

				CTransform* pTransformCom = dynamic_cast<CTransform*>(
					pLayer->Get_Component(ID_DYNAMIC, wstrDoorName, L"Com_Transform"));

				pTransformCom->Set_Pos(vRoomOffset.x + vTerrainOffset.x, 0.f, vRoomOffset.z + vTerrainOffset.z);
			}
		}
	}

	m_mapLayer.insert({ pLayerTag ,pLayer });

	return S_OK;
}

HRESULT CStage::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	// 오브젝트 추가
	CGameObject* pGameObject = nullptr;

	// Terrain
	pGameObject = CTerrain::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"Terrain", pGameObject)))
		return E_FAIL;

	//// Player
	//pGameObject = CPlayer::Create(m_pGraphicDev);
	//if (nullptr == pGameObject)
	//	return E_FAIL;

	//if (FAILED(pLayer->Add_GameObject(L"Player", pGameObject)))
	//	return E_FAIL;

	/* [DEBUG] 총 메쉬 테스트 출력 */
	pGameObject = CGun::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"Gun", pGameObject)))
		return E_FAIL;

	/* 몬스터 소환 */
	_vec3 vTerrainOffset{ 60.f, 0.f, 60.f };

	for (auto& tMapEntity : m_MapData.vecMonsterInfo)
	{
		pGameObject = CMonster::Create(m_pGraphicDev);
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(pLayer->Add_GameObject(tMapEntity.wstrEntityName, pGameObject)))
			return E_FAIL;

	// Monster
	pGameObject = CMonster::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"Monster", pGameObject)))
		return E_FAIL;

		CTransform* pTransformCom = dynamic_cast<CTransform*>(
			pLayer->Get_Component(ID_DYNAMIC, tMapEntity.wstrEntityName, L"Com_Transform"));

		pTransformCom->Set_Pos(tMapEntity.vPos.x + vTerrainOffset.x, tMapEntity.vPos.y, tMapEntity.vPos.z + vTerrainOffset.z);
	}

	m_mapLayer.insert({ pLayerTag ,pLayer });

	return S_OK;
}

HRESULT CStage::Ready_UI_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	// 오브젝트 추가
	CGameObject* pGameObject = nullptr;

	m_mapLayer.insert({ pLayerTag ,pLayer });

	return S_OK;
}

HRESULT CStage::Ready_Light()
{
	D3DLIGHT9	tLight;
	ZeroMemory(&tLight, sizeof(D3DLIGHT9));

	tLight.Type = D3DLIGHT_DIRECTIONAL;
	
	tLight.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLight.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLight.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	
	tLight.Direction = { 1.f, -1.f, 1.f };
	
	if (FAILED(CLightMgr::GetInstance()->Ready_Light(m_pGraphicDev, &tLight, 0)))
		return E_FAIL;

	//tLight.Type = D3DLIGHT_POINT;
	//
	//tLight.Diffuse = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	//tLight.Specular = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	//tLight.Ambient = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	//
	//tLight.Position = { 1.f, 1.f, 1.f };
	//tLight.Range = 5.f;
	//
	//if (FAILED(CLightMgr::GetInstance()->Ready_Light(m_pGraphicDev, &tLight, 1)))
	//	return E_FAIL;
	
	return S_OK;
}


CStage* CStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStage* pStage = new CStage(pGraphicDev);

	if (FAILED(pStage->Ready_Scene()))
	{
		Safe_Release(pStage);
		MSG_BOX("Stage Create Failed");
		return nullptr;
	}

	return pStage;
}

void CStage::Free()
{
	CScene::Free();
}
