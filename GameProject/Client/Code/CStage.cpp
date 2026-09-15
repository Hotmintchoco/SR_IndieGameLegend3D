#include "pch.h"
#include "CStage.h"
#include "CBackGround.h"
#include "CProtoMgr.h"
#include "CPlayer.h"
#include "CTerrain.h"
#include "CDynamicCamera.h"
#include "CCameraMgr.h"
#include "CSkyBox.h"
#include "CLightMgr.h"
#include "CEffect.h"
#include "CGun.h"
#include "CManagement.h"
#include "CFontMgr.h"
#include "CBullet.h"
#include "CDInputMgr.h"
#include "CCollisionMgr.h"
#include "CWorm.h"
#include "CRoomLoadingMgr.h"
#include "CRoomLayer.h"
#include "CLayerContext.h"
#include "CPlayerHpUI.h"
#include "CCrosshair.h"
#include "CSkull.h"
#include "CBoss1.h"
#include "CSpeyeder.h"

CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
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

	int iRoomCnt = CRoomLoadingMgr::GetInstance()->GetRoomTotalCount();
	for (int i = 0; i < iRoomCnt; ++i)
	{
		wstring wstrLayerTag = L"Room_" + to_wstring(i) + L"_Layer";
		if (FAILED(Ready_Room_Layer(wstrLayerTag, i)))
			return E_FAIL;
	}

	if (FAILED(Ready_UI_Layer(L"UI_Layer")))
		return E_FAIL;

	if (FAILED(CCameraMgr::GetInstance()->Ready_Camera(L"Camera_Player_FPV", CAMERA_FPV_PERSPECTIVE, m_pGraphicDev)))
		return E_FAIL;

	if (FAILED(CCameraMgr::GetInstance()->Select_Camera(L"Camera_Player_FPV")))
		return E_FAIL;

	// 충돌 그룹 설정
	Engine::CCollisionMgr::GetInstance()->Check_Group(COLL_PLAYER, COLL_MONSTER);
	Engine::CCollisionMgr::GetInstance()->Check_Group(COLL_PLAYER, COLL_WALL);
	Engine::CCollisionMgr::GetInstance()->Check_Group(COLL_PBULLET, COLL_MONSTER);
	Engine::CCollisionMgr::GetInstance()->Check_Group(COLL_PLAYER, COLL_ITEM);

	return S_OK;
}

_int CStage::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = CScene::Update_Scene(fTimeDelta);

	//Camera Update

	CTransform* pPlayerTrans = static_cast<CTransform*>(Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

	_vec3 vPlayerLook;
	_vec3 vPlayerPos;
	_vec3 vPlayerRight;
	pPlayerTrans->Get_Info(INFO_LOOK, &vPlayerLook);
	pPlayerTrans->Get_Info(INFO_POS, &vPlayerPos);
	pPlayerTrans->Get_Info(INFO_RIGHT, &vPlayerRight);

	CCameraMgr::GetInstance()->Update_Camera(fTimeDelta, vPlayerLook, vPlayerPos, vPlayerRight);

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

	/* 현재 씬, 레이어 정보를 전역으로 주입 */
	CLayerContext ctx(pLayer, this);

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

	/* 현재 씬, 레이어 정보를 전역으로 주입 */
	CLayerContext ctx(pLayer, this);

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

	// Gun
	pGameObject = CGun::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"Gun", pGameObject)))
		return E_FAIL;

	
	// Monster
	TCHAR		szFileName[128] = L"";

	pGameObject = CSkull::Create(m_pGraphicDev);
	static_cast<CMonster*>(pGameObject)->Set_Pos(55, 0, 55);
	if (nullptr == pGameObject)
		return E_FAIL;
	wsprintf(szFileName, L"Skull_%d", CMonster::iMonsterIdx);
	if (FAILED(pLayer->Add_GameObject(szFileName, pGameObject)))
		return E_FAIL;

	pGameObject = CBoss1::Create(m_pGraphicDev);
	static_cast<CMonster*>(pGameObject)->Set_Pos(65, 0, 55);
	if (nullptr == pGameObject)
		return E_FAIL;
	wsprintf(szFileName, L"Boss1_%d", CMonster::iMonsterIdx);
	if (FAILED(pLayer->Add_GameObject(szFileName, pGameObject)))
		return E_FAIL;

	pGameObject = CSpeyeder::Create(m_pGraphicDev);
	static_cast<CMonster*>(pGameObject)->Set_Pos(64, 0, 56);
	if (nullptr == pGameObject)
		return E_FAIL;
	wsprintf(szFileName, L"Speyeder_%d", CMonster::iMonsterIdx);
	if (FAILED(pLayer->Add_GameObject(szFileName, pGameObject)))
		return E_FAIL;

	pGameObject = CSpeyeder::Create(m_pGraphicDev);
	static_cast<CMonster*>(pGameObject)->Set_Pos(64, 0, 57);
	if (nullptr == pGameObject)
		return E_FAIL;
	wsprintf(szFileName, L"Speyeder_%d", CMonster::iMonsterIdx);
	if (FAILED(pLayer->Add_GameObject(szFileName, pGameObject)))
		return E_FAIL;

	//map<const _tchar*, CLayer*>* a = &m_mapLayer;

	//pGameObject = CWorm::Create(m_pGraphicDev, &m_mapLayer);
	//if (nullptr == pGameObject)
	//	return E_FAIL;

	//wsprintf(szFileName, L"Wrom_Boby_0_%d", CMonster::iMonsterIdx);

	//if (FAILED(pLayer->Add_GameObject(szFileName, pGameObject)))
	//	return E_FAIL;

	//m_mapLayer.insert({ pLayerTag ,pLayer });
	m_mapLayer.insert({ pLayerTag ,pLayer });

	return S_OK;
}

HRESULT CStage::Ready_Room_Layer(const wstring& wstrLayerTag, int iRoomIdx)
{
	CLayer* pLayer = CRoomLayer::Create(iRoomIdx);
	if (nullptr == pLayer)
		return E_FAIL;

	/* 현재 씬, 레이어 정보를 전역으로 주입 */
	CLayerContext ctx(pLayer, this);

	if (FAILED(static_cast<CRoomLayer*>(pLayer)->SpawnRoom()))
	{
		return E_FAIL;
	}

	m_mapLayer.insert({ wstrLayerTag, pLayer });

	return S_OK;
}

HRESULT CStage::Ready_UI_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	/* 현재 씬, 레이어 정보를 전역으로 주입 */
	CLayerContext ctx(pLayer, this);

	CUI* pUI = nullptr;

	// Crosshair
	pUI = CCrosshair::Create(m_pGraphicDev);
	if (nullptr == pUI)
		return E_FAIL;

	_vec2 vPos{ WINCX >> 1, WINCY >> 1 };
	pUI->Set_Pos(vPos);

	if (FAILED(pLayer->Add_GameObject(L"Crosshair", pUI)))
		return E_FAIL;

	// Hp
	const _int iHpCount = 3;
	const _float fStartX = 20.f;
	const _float fStartY = 20.f;
	const _float fIconSize = 15.f;  // CPlayerHpUI::Ready_GameObject()의 Set_Scale과 동일
	const _float fGap = 15.f;

	for (_int i = 0; i < iHpCount; ++i)
	{
		pUI = CPlayerHpUI::Create(m_pGraphicDev);
		if (nullptr == pUI)
			return E_FAIL;

		_vec2 vPos{ fStartX + i * (fIconSize + fGap), fStartY };
		pUI->Set_Pos(vPos);

		wstring wstrTag = L"PlayerHp_" + to_wstring(i);
		if (FAILED(pLayer->Add_GameObject(wstrTag.c_str(), pUI)))
			return E_FAIL;
	}

	m_mapLayer.insert({ pLayerTag, pLayer });

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
