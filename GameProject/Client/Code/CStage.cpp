#include "pch.h"
#include "CStage.h"
#include "CBackGround.h"
#include "CProtoMgr.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CTerrain.h"
//#include "CDynamicCamera.h"
#include "CCameraMgr.h"
#include "CSkyBox.h"
#include "CLightMgr.h"
#include "CEffect.h"
#include "CFontMgr.h"
#include "CBullet.h"
#include "CDInputMgr.h"


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

	if (FAILED(Ready_UI_Layer(L"UI_Layer")))
		return E_FAIL;

	if (FAILED(CCameraMgr::GetInstance()->Ready_Camera(L"Camera_Player_FPV", CAMERA_FPV_PERSPECTIVE, m_pGraphicDev)))
		return E_FAIL;

	if (FAILED(CCameraMgr::GetInstance()->Select_Camera(L"Camera_Player_FPV")))
		return E_FAIL;

	return S_OK;
}

_int CStage::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = CScene::Update_Scene(fTimeDelta);

	CCameraMgr::GetInstance()->Update_Camera(fTimeDelta);

	if (CDInputMgr::GetInstance()->Mouse_Down(DIM_LB))
	{
		CGameObject* pGameObject = nullptr;
		_vec3	vPos_Player;
		_vec3	vLook_Cam;
		static_cast<CTransform*>(Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"))->Get_Info(INFO_POS, &vPos_Player);
		CCameraMgr::GetInstance()->Get_CamLook(&vLook_Cam);

		pGameObject = CBullet::Create(m_pGraphicDev, &vPos_Player, &vLook_Cam);

		m_pLogicLayer->Add_GameObject(L"Bullet", pGameObject);
	}

	return iExit;
}

void CStage::LateUpdate_Scene(const _float& fTimeDelta)
{
	CScene::LateUpdate_Scene(fTimeDelta);
}

void CStage::Render_Scene()
{

	_vec2	vPos_DebugUI_PlayerX{ 100.f, 100.f };
	_vec2	vPos_DebugUI_PlayerY{ 100.f, 120.f };
	_vec2	vPos_DebugUI_PlayerZ{ 100.f, 140.f };
	_vec2	vPos_DebugUI_JumpState{ 100.f, 160.f };
	_vec2	vPos_DebugUI_CameraAngle{ 100.f, 180.f };


	_vec3	vPos_Player;
	static_cast<CTransform*>(Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"))->Get_Info(INFO_POS, &vPos_Player);
	
	wstring wPlayerInfoX = L"PLAYER X : " + to_wstring(vPos_Player.x);
	wstring wPlayerInfoY = L"PLAYER Y : " + to_wstring(vPos_Player.y);
	wstring wPlayerInfoZ = L"PLAYER Z : " + to_wstring(vPos_Player.z);

	CFontMgr::GetInstance()->Render_Font(L"Font_Jinji", wPlayerInfoX.c_str(), &vPos_DebugUI_PlayerX, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	CFontMgr::GetInstance()->Render_Font(L"Font_Jinji", wPlayerInfoY.c_str(), &vPos_DebugUI_PlayerY, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	CFontMgr::GetInstance()->Render_Font(L"Font_Jinji", wPlayerInfoZ.c_str(), &vPos_DebugUI_PlayerZ, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));


	_int iJumpState = (static_cast<CPlayer*>(Get_GameObject(L"GameLogic_Layer", L"Player")))->Get_JumpState();
	wstring wsJumpInfo = L"JUMPSTATE : ";
	switch (iJumpState)
	{
	case JUMP_NOT : 
		wsJumpInfo += L"JUMP_NOT";
		break;
	case JUMP_PARABOLIC : 
		wsJumpInfo += L"JUMP_PARABOLIC";
		break;
	case JUMP_FREEFALL : 
		wsJumpInfo += L"JUMP_FREEFALL";
		break;
	}

	CFontMgr::GetInstance()->Render_Font(L"Font_Jinji", wsJumpInfo.c_str() , &vPos_DebugUI_JumpState, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	_float fAngle;
	CCameraMgr::GetInstance()->Get_CameraAngle(&fAngle);
	wstring wAngleInfo = L"Camera Angle : " + to_wstring(fAngle) + L"°";

	CFontMgr::GetInstance()->Render_Font(L"Font_Jinji", wAngleInfo.c_str(), &vPos_DebugUI_CameraAngle, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

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
	_vec3   vEye{ 0.f, 10.f, -10.f };
	_vec3   vAt{ 0.f, 0.f, 1.f };
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
	
	// BackGround

	pGameObject = CPlayer::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;
	
	if (FAILED(pLayer->Add_GameObject(L"Player", pGameObject)))
		return E_FAIL;
	
	
	pGameObject = CMonster::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;
	
	if (FAILED(pLayer->Add_GameObject(L"Monster", pGameObject)))
		return E_FAIL;
	

	// Effect

	for (int i = 0; i < 50; ++i)
	{
		pGameObject = CEffect::Create(m_pGraphicDev);
	
		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(pLayer->Add_GameObject(L"Effect", pGameObject)))
			return E_FAIL;
	}

	m_mapLayer.insert({ pLayerTag ,pLayer });

	m_pLogicLayer = pLayer;

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
