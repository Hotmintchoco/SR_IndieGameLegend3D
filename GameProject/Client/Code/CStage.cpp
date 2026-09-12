#include "pch.h"
#include "CStage.h"
#include "CBackGround.h"
#include "CProtoMgr.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CTerrain.h"
#include "CDynamicCamera.h"
#include "CCameraMgr.h"
#include "CSkyBox.h"
#include "CLightMgr.h"
#include "CEffect.h"
#include "CGun.h"
#include "CManagement.h"
#include "CRoom.h"
#include "CRoomLoadingMgr.h"
#include "CFontMgr.h"
#include "CBullet.h"
#include "CDInputMgr.h"
#include "CCollisionMgr.h"
#include "CTile.h"
#include "CWall.h"
#include "CFog.h"

CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev), m_fLastShotTime(0.f), m_fReloadTime(0.f), m_iAmmo(13)
{
}

CStage::~CStage()
{
}

HRESULT CStage::Ready_Scene()
{
	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
		return E_FAIL;

	if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(L"UI_Layer")))
		return E_FAIL;

	if (FAILED(CCameraMgr::GetInstance()->Ready_Camera(L"Camera_Player_FPV", CAMERA_FPV_PERSPECTIVE, m_pGraphicDev)))
		return E_FAIL;

	if (FAILED(CCameraMgr::GetInstance()->Select_Camera(L"Camera_Player_FPV")))
		return E_FAIL;

	// 충돌 그룹 설정
	Engine::CCollisionMgr::GetInstance()->Check_Group(Engine::COLL_PLAYER, Engine::COLL_MONSTER);
	Engine::CCollisionMgr::GetInstance()->Check_Group(Engine::COLL_PLAYER, Engine::COLL_WALL);
	Engine::CCollisionMgr::GetInstance()->Check_Group(Engine::COLL_PBULLET, Engine::COLL_MONSTER);

	return S_OK;
}

HRESULT CStage::PostInitialize()
{
	for (auto pRoom : m_vecRoom)
	{
		if (FAILED(pRoom->PostInitialize()))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

_int CStage::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = CScene::Update_Scene(fTimeDelta);

	CCameraMgr::GetInstance()->Update_Camera(fTimeDelta);

	_int iAmmoMax = 13;
	_float fShootCoolTime = 0.4f;
	_float fReloadCoolTime = 1.f;
	m_fLastShotTime += fTimeDelta;

	if (m_fReloadTime > fReloadCoolTime)
	{
		m_fReloadTime = 0.f;
		m_fLastShotTime = fShootCoolTime;
		m_iAmmo = iAmmoMax;
	}
	else if (m_fReloadTime > 0.f)
	{
		m_fReloadTime += fTimeDelta;
	}

	if ((CDInputMgr::GetInstance()->Mouse_Press(DIM_LB)) && (m_fLastShotTime >= fShootCoolTime) && (m_iAmmo > 0) && (m_fReloadTime == 0))
	{
		m_fLastShotTime = 0.f;
		m_iAmmo--;

		CGameObject* pGameObject = nullptr;
		_vec3	vPos_Player;
		_vec3	vRight;
		_vec3	vForword;
		_vec3	vUp;
		static_cast<CTransform*>(Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"))->Get_Info(INFO_POS, &vPos_Player);
		static_cast<CTransform*>(Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"))->Get_Info(INFO_RIGHT, &vRight);
		CCameraMgr::GetInstance()->Get_CamLook(&vForword);
		D3DXVec3Cross(&vUp, &vForword, &vRight);
		D3DXVec3Normalize(&vUp, &vUp);

		_vec3	vBullet_From = vPos_Player + (vRight * 0.5f) + (vForword * 0.5f) + (vUp * -0.5f); // 총구위치 이동
		_vec3	vBullet_To = vPos_Player + (vForword * 10.f); // 크로스헤어 도달점

		_vec3	vBullet_Look = vBullet_To - vBullet_From;
		D3DXVec3Normalize(&vBullet_Look, &vBullet_Look);

		pGameObject = CBullet::Create(m_pGraphicDev, &vBullet_From, &vBullet_Look);

		auto iter = m_mapLayer.find(L"GameLogic_Layer");
		if (iter != m_mapLayer.end())
		{
			iter->second->Add_GameObject(L"Bullet", pGameObject);
		}
	}
	else if ((m_iAmmo == 0))
	{
		if (CDInputMgr::GetInstance()->Key_Down(DIK_R))
		{
			m_fReloadTime += fTimeDelta;
		}
	}

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
	/* 이것도 수정 부탁 */

	//_vec2	vPos_DebugUI_Ammo{ 100.f, 120.f };
	//_vec2	vPos_DebugUI_Reload{ 100.f, 140.f };

	//wstring wAmmoInfo = L"AMMO : " + to_wstring(m_iAmmo) + L" / 13";
	//CFontMgr::GetInstance()->Render_Font(L"Font_Jinji", wAmmoInfo.c_str(), &vPos_DebugUI_Ammo, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	//if (m_iAmmo == 0)
	//{
	//	if (m_fReloadTime == 0.f)
	//	{
	//		if ((int)(m_fLastShotTime * 5) % 2 == 0)
	//		{
	//			CFontMgr::GetInstance()->Render_Font(L"Font_Jinji", L"OUT OF AMMO. PRESS R TO RELOAD.", &vPos_DebugUI_Reload, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	//		}
	//	}
	//	else
	//	{
	//		CFontMgr::GetInstance()->Render_Font(L"Font_Jinji", L"Reloading...", &vPos_DebugUI_Reload, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	//	}
	//}
}

HRESULT CStage::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	// 오브젝트 추가
	CGameObject* pGameObject = nullptr;

	/*
	// DynamicCamera
	_vec3   vEye{ 60.f, 10.f, 50.f };
	_vec3   vAt{ 60.f, 0.f, 61.f };
	_vec3   vUp{ 0.f, 1.f, 0.f };

	pGameObject = CDynamicCamera::Create(m_pGraphicDev, 
										&vEye, &vAt,&vUp);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"DynamicCamera", pGameObject)))
		return E_FAIL;

	*/


	// SkyBox
	pGameObject = CSkyBox::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"SkyBox", pGameObject)))
		return E_FAIL;

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

	// Player
	pGameObject = CPlayer::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"Player", pGameObject)))
		return E_FAIL;

	/* [DEBUG] 총 메쉬 테스트 출력 */
	pGameObject = CGun::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"Gun", pGameObject)))
		return E_FAIL;

	/* 방 출력 */
	for (int i = 0; i < CRoomLoadingMgr::GetInstance()->GetRoomTotalCount(); ++i)
	{
		pGameObject = CRoom::Create(m_pGraphicDev, i);
		if (nullptr == pGameObject)
			return E_FAIL;

		wstring wstrRoomName = L"Room_" + to_wstring(i);

		if (FAILED(pLayer->Add_GameObject(wstrRoomName, pGameObject)))
			return E_FAIL;

		m_vecRoom.push_back(static_cast<CRoom*>(pGameObject));
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
