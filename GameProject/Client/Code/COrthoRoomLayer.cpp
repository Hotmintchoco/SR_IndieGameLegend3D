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
		CClientCameraMgr::GetInstance()->Select_Camera(CLIENT_CAMERA_TYPE::ORTHO);
		m_pOrthoCamera->UpdateView(GetCenterPos() + _vec3{ 0.f, 14.f, -6.f }, GetCenterPos());
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
		}
		//m_pOrthoCamera->Orbit(
		//	GetCenterPos() + _vec3{ 0.f, 0.f, -10.f },
		//	GetCenterPos() + _vec3{ 0.f, 10.f, 0.f },
		//	GetCenterPos(),
		//	2.f
		//);
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
			// if (ITerrain* p = dynamic_cast<ITerrain*>(pObj))         m_vecTerrain.push_back(p);

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
			{ 1,          2 },   // 왼쪽 2칸
			{ iMidX - 1,  3 },   // 가운데 3칸
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

	return S_OK;
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