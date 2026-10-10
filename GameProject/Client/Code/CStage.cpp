#include "pch.h"
#include "CShaderEffectMgr.h"
#include "CStage.h"

/* 매니저 */
#include "CManagement.h"
#include "CProtoMgr.h"
#include "CDInputMgr.h"
#include "CCollisionMgr.h"
#include "CSoundMgr.h"

/* 기본 지형 */
#include "CBackGround.h"
#include "CTerrain.h"
#include "CSkyBox.h"

/* 방 레이어*/
#include "CRoomLoadingMgr.h"
#include "CRoomLayer.h"
#include "CLayerContext.h"

/* 오브젝트 */
#include "CPlayer.h"
#include "CWeaponSystem.h"
#include "CRayCaster.h"
#include "CGameStatus.h"

/* UI */
#include "CUIMgr.h"
#include "CCrosshair.h"
#include "CDirectionUI.h"
#include "CMinimapUI.h"
#include "CGaugeUI.h"
#include "CHitCreenUI.h"

/* 카메라 */
#include "CCamera.h"
#include "CClientCameraMgr.h"
#include "CPlayerCamera.h"
#include "CDynamicCamera.h"
#include "CCinematicCamera.h"

/* 씬 전환 */
#include "CMiniGame.h"
#include "CMiniGame1.h"

CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)

{
}

CStage::~CStage()
{
}

HRESULT CStage::Ready_Scene()
{
	if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
		return E_FAIL;

	int iRoomCnt = CRoomLoadingMgr::GetInstance()->GetRoomTotalCount();
	for (int i = 0; i < iRoomCnt; ++i)
	{
		wstring wstrLayerTag = L"Room_" + to_wstring(i) + L"_Layer";
		if (FAILED(Ready_Room_Layer(wstrLayerTag, i)))
			return E_FAIL;
	}

	m_pCurrentRoomLayer = GetRoomLayerFromIndex(m_iStartRoomIndex);

	if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(L"UI_Layer")))
		return E_FAIL;

	if (FAILED(Ready_Screen_Layer(L"Screen_Layer")))
		return E_FAIL;

	if (FAILED(Ready_Camera()))
		return E_FAIL;

	// 충돌 그룹 설정
	Engine::CCollisionMgr::GetInstance()->Check_Group(COLL_PLAYER, COLL_MONSTER);
	Engine::CCollisionMgr::GetInstance()->Check_Group(COLL_PLAYER, COLL_OBSTACLE);
	Engine::CCollisionMgr::GetInstance()->Check_Group(COLL_MBULLET, COLL_OBSTACLE);
	Engine::CCollisionMgr::GetInstance()->Check_Group(COLL_PLAYER, COLL_ITEM);
	Engine::CCollisionMgr::GetInstance()->Check_Group(COLL_EXPLODE, COLL_OBSTACLE);
	Engine::CCollisionMgr::GetInstance()->Check_Group(COLL_EXPLODE, COLL_PLAYER);
	Engine::CCollisionMgr::GetInstance()->Check_Group(COLL_EXPLODE, COLL_MONSTER);
	Engine::CCollisionMgr::GetInstance()->Check_Group(COLL_MONSTER, COLL_OBSTACLE);
	Engine::CCollisionMgr::GetInstance()->Check_Group(COLL_MONSTER, COLL_MONSTER);
	Engine::CCollisionMgr::GetInstance()->Check_Group(COLL_MBULLET, COLL_PLAYER);

	/* 방 로직 */
	Engine::CCollisionMgr::GetInstance()->Check_Group(COLL_ROOMLOGIC, COLL_PLAYER);

	/* 투사체와의 충돌 */
	Engine::CCollisionMgr::GetInstance()->Check_Group(COLL_PROJECTILE, COLL_MONSTER);
	Engine::CCollisionMgr::GetInstance()->Check_Group(COLL_PROJECTILE, COLL_OBSTACLE);

	int iBiome = CRoomLoadingMgr::GetInstance()->GetRoomData(m_iStartRoomIndex)->iBiome;
	TBGMTrack tTrack = CRoomLoadingMgr::GetInstance()->GetBiomeInfo(iBiome).tBGMTrack;
	CSoundMgr::GetInstance()->PlayBGM(tTrack);
	CSoundMgr::GetInstance()->SetBGMVolume(0.2f);
	CSoundMgr::GetInstance()->SetSFXVolume(0.5f);

	return S_OK;
}

_int CStage::Update_Scene(_float fTimeDelta)
{
	CClientCameraMgr::GetInstance()->Update_Camera(fTimeDelta);
	_int iExit = CScene::Update_Scene(fTimeDelta);

	CUIMgr::GetInstance()->Update_UI();

	// Scene Change
	if (CDInputMgr::GetInstance()->Key_Down(DIK_F1))
	{
		CScene* pMiniGame = CMiniGame::Create(m_pGraphicDev);
		if (nullptr == pMiniGame)
			return E_FAIL;

		if (FAILED(CManagement::GetInstance()->Change_Scene(1, pMiniGame)))
		{
			Safe_Release(pMiniGame);
			MSG_BOX("MiniGame Create Failed");
			return -1;
		}
		pMiniGame->Update_Scene(fTimeDelta);
	}

	if (CDInputMgr::GetInstance()->Key_Down(DIK_F3))
	{
		CScene* pMiniGame = CMiniGame1::Create(m_pGraphicDev);
		if (nullptr == pMiniGame)
			return E_FAIL;

		if (FAILED(CManagement::GetInstance()->Change_Scene(1, pMiniGame)))
		{
			Safe_Release(pMiniGame);
			MSG_BOX("MiniGame Create Failed");
			return -1;
		}
		pMiniGame->Update_Scene(fTimeDelta);
	}

	if (m_bOnPlayerDead)
	{
		m_fLeftReviveTime -= fTimeDelta;
		if (m_fLeftReviveTime <= 0.f)
		{
			m_bOnPlayerDead = false;
			_vec3 vRevivePos = GetRoomLayerFromIndex(m_iPrevRoomIndex)->GetCenterPos();
			m_pPlayer->GetTransform()->Set_Pos(vRevivePos);
			m_pPlayer->Revive();
		}
	}

	return iExit;
}

void CStage::LateUpdate_Scene(_float fTimeDelta)
{
	CClientCameraMgr::GetInstance()->LateUpdate_Camera(fTimeDelta);
	CScene::LateUpdate_Scene(fTimeDelta);

	Engine::CCollisionMgr::GetInstance()->Update_Collision();
	Engine::CCollisionMgr::GetInstance()->Clear_ColliderList();

}

HRESULT CStage::Ready_Camera()
{
	// 플레이어 정보
	CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Get_Component(ID_DYNAMIC, 
										L"GameLogic_Layer", L"Player", L"Com_Transform"));

	auto* pCameraMgr = CClientCameraMgr::GetInstance();
	pCameraMgr->Free();

	CCamera* pCamera = nullptr;

	_vec3 vEye, vAt;
	_vec3 vUp{ 0.f, 1.f, 0.f };

	pPlayerTransformCom->Get_Info(INFO_POS, &vEye);
	pPlayerTransformCom->Get_Info(INFO_LOOK, &vAt);

	pCamera = CDynamicCamera::Create(m_pGraphicDev, &vEye, &vAt, &vUp);
	if (!pCamera) 
		return E_FAIL;

	if (FAILED(pCameraMgr->Add_Camera(CLIENT_CAMERA_TYPE::FREE, pCamera)))
	{
		pCamera->Release();
		return E_FAIL;
	}

	CPlayerCamera* pPlayerCamera = CPlayerCamera::Create(m_pGraphicDev, pPlayerTransformCom);

	if (!pPlayerCamera)
		return E_FAIL;

	if (FAILED(pCameraMgr->Add_Camera(CLIENT_CAMERA_TYPE::PLAYER, pPlayerCamera)))
	{
		pCamera->Release();
		return E_FAIL;
	}

	GetPlayer()->SetCamera(pPlayerCamera);


	pCamera = CCinematicCamera::Create(m_pGraphicDev);

	if (!pCamera)
		return E_FAIL;

	if (FAILED(pCameraMgr->Add_Camera(CLIENT_CAMERA_TYPE::CINEMATIC, pCamera)))
	{
		pCamera->Release();
		return E_FAIL;
	}

	if (FAILED(pCameraMgr->Select_Camera(CLIENT_CAMERA_TYPE::PLAYER)))
		return E_FAIL;

	pCameraMgr->LateUpdate_Camera(0.f);

	return S_OK;
}

void CStage::OnEnter()
{
    auto* pRoomMgr = CRoomLoadingMgr::GetInstance();
    if (m_pCurrentRoomLayer && m_iCurrentRoomIndex >= 0)
    {
        const auto* pRoom = pRoomMgr->GetRoomData(m_iCurrentRoomIndex);
        ApplyRoomShader(pRoomMgr->GetBiomeInfo(pRoom->iBiome).eType);
    }
	if (SUCCEEDED(CClientCameraMgr::GetInstance()->Select_Camera(CLIENT_CAMERA_TYPE::PLAYER)))
		CClientCameraMgr::GetInstance()->LateUpdate_Camera(0.f);
}

void CStage::OnExit()
{
    CShaderEffectMgr::GetInstance()->Set_PostEffect(POST_EFFECT::NONE);
	CSoundMgr::GetInstance()->StopBGM();

}

HRESULT CStage::Add_GameObject(const wstring& pObjTag, CGameObject* pGameObject)
{
	/* 현재 위치한 방에 오브젝트를 소환 */
	if (FAILED(m_pCurrentRoomLayer->Add_GameObject(pObjTag, pGameObject)))
		return E_FAIL;

	return S_OK;
}

void CStage::OnPlayerDead()
{
	m_pCurrentRoomLayer->ResetState();

	/* 부활 타이머 */
	m_bOnPlayerDead = true;
	m_fLeftReviveTime = m_fReviveTime;
}

void CStage::UpdatePlayerPosition(const _vec3& vPos)
{
	m_vPlayerPos = vPos;
	CheckRoomChanged();
}

CRoomLayer* CStage::GetRoomLayerFromIndex(int iIndex)
{
	wstring wstrRoomLayerKey = L"Room_" + to_wstring(iIndex) + L"_Layer";
	CRoomLayer* pLayer = static_cast<CRoomLayer*>(Get_Layer(wstrRoomLayerKey));
	if (pLayer)
	{
		return pLayer;
	}
	else
	{
		assert(0);
		return nullptr;
	}
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

	// Game Status
	m_pStatus = CGameStatus::Create(m_pGraphicDev);
	if (nullptr == m_pStatus)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"GameStatus", m_pStatus)))
		return E_FAIL;

	m_pStatus->SetStage(this);

	// Terrain
	pGameObject = CTerrain::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"Terrain", pGameObject)))
		return E_FAIL;

	// Player
	m_pPlayer = CPlayer::Create(m_pGraphicDev);
	if (nullptr == m_pPlayer)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"Player", m_pPlayer)))
		return E_FAIL;

	m_pPlayer->GetTransform()->Set_Pos(GetRoomLayerFromIndex(m_iStartRoomIndex)->GetCenterPos());

	// Weapon System
	CWeaponSystem* pWeaponSystem = CWeaponSystem::Create(m_pGraphicDev);
	if (nullptr == pWeaponSystem)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"WeaponSystem", pWeaponSystem)))
		return E_FAIL;
	m_pPlayer->SetWeaponSystem(pWeaponSystem);

	// Ray Caster
	pGameObject = CRayCaster::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"RayCaster", pGameObject)))
		return E_FAIL;

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
	_int iCountMax = 3;
	_float fStartX = 25.f;
	_float fStartY = 28.f;
	_float fIconSize = 17.5f;
	_float fGap = 25.f;

	for (_int i = 0; i < iCountMax; ++i)
	{
		pUI = CUI::Create(m_pGraphicDev, L"Proto_HpUITexture");
		if (nullptr == pUI)
			return E_FAIL;

		vPos = { fStartX + i * (fIconSize + fGap), fStartY };
		pUI->Set_Pos(vPos);
		pUI->Set_Size({ fIconSize + 2.5f, fIconSize });
		pUI->Set_Texture(4);

		wstring wstrTag = L"PlayerHp_" + to_wstring(i);
		if (FAILED(pLayer->Add_GameObject(wstrTag.c_str(), pUI)))
			return E_FAIL;
	}

	// Gem
	fIconSize = 20.f;
	fStartX = 675.f;
	pUI = CUI::Create(m_pGraphicDev, L"Proto_Item_Gem_Texture");

	vPos = { fStartX,  fStartY };
	pUI->Set_Pos(vPos);
	pUI->Set_Size({ fIconSize, fIconSize });

	if (FAILED(pLayer->Add_GameObject(L"Gem", pUI)))
		return E_FAIL;

	// Gem Cnt
	fStartX += fGap + 15.f;
	fGap = 4.f;
	for (int i = 0; i < iCountMax; ++i)
	{
		pUI = CUI::Create(m_pGraphicDev, L"Proto_NumberTexture");
		if (nullptr == pUI)
			return E_FAIL;

		vPos = { fStartX + i * (fIconSize + fGap), fStartY };
		pUI->Set_Pos(vPos);
		pUI->Set_Size({ fIconSize + 2.5f, fIconSize });

		wstring wstrTag = L"GemNum_" + to_wstring(i);
		if (FAILED(pLayer->Add_GameObject(wstrTag.c_str(), pUI)))
			return E_FAIL;
	}

	// Direction UI
	pUI = CDirectionUI::Create(m_pGraphicDev);
	if (nullptr == pUI)
		return E_FAIL;

	pUI->Set_Pos(WINCX - 90.f, 410.f, 0.1f);

	if (FAILED(pLayer->Add_GameObject(L"DirectionUI", pUI)))
		return E_FAIL;

	// Hud Minimap
	pUI = CUI::Create(m_pGraphicDev, L"Proto_HudMapTexture");
	if (nullptr == pUI)
		return E_FAIL;

	pUI->Set_Pos(WINCX - 90.f, 480.f, 0.f);
	pUI->Set_Size({ 76.f, 92.f });

	if (FAILED(pLayer->Add_GameObject(L"HudMiniMap", pUI)))
		return E_FAIL;

	// Minimap
	pUI = CMinimapUI::Create(m_pGraphicDev);
	if (nullptr == pUI)
		return E_FAIL;
	
	pUI->Set_Pos(WINCX - 90.f, 498.f, 0.f);
	pUI->Set_Size({ 128.f, 128.f });
	
	if (FAILED(pLayer->Add_GameObject(L"MiniMap", pUI)))
		return E_FAIL;

	// Hud Attack Info
	pUI = CUI::Create(m_pGraphicDev, L"Proto_HudAttackInfoTexture");
	if (nullptr == pUI)
		return E_FAIL;

	pUI->Set_Pos(182.f, WINCY - 60.f, 0.5f);
	pUI->Set_Size({ 175.f, 38.5f });

	if (FAILED(pLayer->Add_GameObject(L"AttackInfo", pUI)))
		return E_FAIL;

	// Hud Attack Info Ammo
	pUI = CGaugeUI::Create(m_pGraphicDev, L"Proto_AmmoTexture");
	if (nullptr == pUI)
		return E_FAIL;

	pUI->Set_Pos(222.f, WINCY - 60.f, 0.6f);
	pUI->Set_Size({ 125.f, 18.f });
	CUIMgr::GetInstance()->Add_UI(UI_SPECIAL, pUI);

	if (FAILED(pLayer->Add_GameObject(L"AmmoInfo", pUI)))
		return E_FAIL;

	// Hud Attack Info Ult
	pUI = CGaugeUI::Create(m_pGraphicDev, L"Proto_UltTexture", false);
	if (nullptr == pUI)
		return E_FAIL;

	pUI->Set_Pos(21.f, WINCY - 58.5f, 0.4f);
	pUI->Set_Size({ 6.5f, 32.f });
	CUIMgr::GetInstance()->Add_UI(UI_ULTIMATE, pUI);

	if (FAILED(pLayer->Add_GameObject(L"UltInfo", pUI)))
		return E_FAIL;

	// Hud Attack Info Skill
	pUI = CUI::Create(m_pGraphicDev, L"Proto_SkillTexture");
	if (nullptr == pUI)
		return E_FAIL;

	pUI->Set_Pos(63.f, WINCY - 60.f, 0.4f);
	pUI->Set_Size({ 28.f, 28.f });
	CUIMgr::GetInstance()->Add_UI(UI_SPECIAL, pUI);

	if (FAILED(pLayer->Add_GameObject(L"SkillInfo", pUI)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag, pLayer });

	// Hud Skill Enable UI
	pUI = CUI::Create(m_pGraphicDev, L"Proto_SkillEnableTexture");
	if (nullptr == pUI)
		return E_FAIL;

	pUI->Set_Pos(63.f, WINCY - 60.f, 0.3f);
	pUI->Set_Size({ 28.f, 28.f });
	pUI->Set_SyncSwitchToActive(true);

	CUIMgr::GetInstance()->Add_UI(UI_SPECIAL, pUI);

	if (FAILED(pLayer->Add_GameObject(L"SkillEnableUI", pUI)))
		return E_FAIL;

	// Boss Font
	pUI = CUI::Create(m_pGraphicDev, L"Proto_BossFontTexture");
	if (nullptr == pUI)
		return E_FAIL;

	pUI->Set_Pos(280.f, 32.f, 0.f);
	pUI->Set_Size({ 64.f, 28.f });
	pUI->Set_IsActive(false);

	CUIMgr::GetInstance()->Add_UI(UI_BOSS, pUI);

	if (FAILED(pLayer->Add_GameObject(L"BossFont", pUI)))
		return E_FAIL;

	// Boss HpBar
	pUI = CUI::Create(m_pGraphicDev, L"Proto_BossHpBarTexture");
	if (nullptr == pUI)
		return E_FAIL;

	pUI->Set_Pos(460.f, 32.f, 0.1f);
	pUI->Set_Size({ 108.f, 16.f });
	pUI->Set_IsActive(false);

	CUIMgr::GetInstance()->Add_UI(UI_BOSS, pUI);

	if (FAILED(pLayer->Add_GameObject(L"BossHpBar", pUI)))
		return E_FAIL;

	// Boss HpUI
	pUI = CGaugeUI::Create(m_pGraphicDev, L"Proto_RedTexture");
	if (nullptr == pUI)
		return E_FAIL;

	pUI->Set_Pos(460.f, 32.f, 0.f);
	pUI->Set_Size({ 102.f, 13.f });
	pUI->Set_IsActive(false);

	CUIMgr::GetInstance()->Add_UI(UI_BOSS, pUI);

	if (FAILED(pLayer->Add_GameObject(L"BossHpUI", pUI)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

HRESULT CStage::Ready_Screen_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CUI* pUI = nullptr;

	// Hud Hit Effect UI
	pUI = CHitCreenUI::Create(m_pGraphicDev, L"Proto_HitScreenTexture");
	if (nullptr == pUI)
		return E_FAIL;

	pUI->Set_Pos(WINCX >> 1, WINCY >> 1, 0.f);
	pUI->Set_Size({ WINCX >> 1, WINCY >> 1 });

	if (FAILED(pLayer->Add_GameObject(L"HitScreen", pUI)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag, pLayer });

	return S_OK;
}

void CStage::CheckRoomChanged()
{
	int m_iRoomIndex = CalculateRoomIndexFromPlayerPosition();

	if (m_iRoomIndex != m_iCurrentRoomIndex)
	{
		m_iPrevRoomIndex = m_iCurrentRoomIndex;
		m_iCurrentRoomIndex = m_iRoomIndex;
		if(m_pStatus) m_pStatus->UpdateCurrentRoomIndex(m_iCurrentRoomIndex);
		wstring wstrRoomLayerKey = L"Room_" + to_wstring(m_iCurrentRoomIndex) + L"_Layer";
		CRoomLayer* pLayer = static_cast<CRoomLayer*>(CManagement::GetInstance()->Get_Layer(wstrRoomLayerKey.c_str()));

		m_pCurrentRoomLayer = pLayer;
		if (m_pCurrentRoomLayer)
		{
			m_pCurrentRoomLayer->ApplyDarkness();

			auto* pMgr = CRoomLoadingMgr::GetInstance();
			const auto* pRoom = pMgr->GetRoomData(m_iCurrentRoomIndex);
			const auto biome = pMgr->GetBiomeInfo(pRoom->iBiome);

			ApplyRoomShader(biome.eType);

			if (pRoom->bBossRoom)
				CUIMgr::GetInstance()->EnterBossScreen();

			int iBiome = CRoomLoadingMgr::GetInstance()->GetRoomData(m_iCurrentRoomIndex)->iBiome;
			TBGMTrack tTrack = CRoomLoadingMgr::GetInstance()->GetBiomeInfo(iBiome).tBGMTrack;
			CSoundMgr::GetInstance()->PlayBGM(tTrack);
		}
	}
}

int CStage::CalculateRoomIndexFromPlayerPosition()
{
	const _vec3 vCenter = CRoomLoadingMgr::GetInstance()->GetCenterRoomPosition();
	const _vec3 vRoomSize = CRoomLoadingMgr::GetInstance()->GetOuterRoomSize();
	const int iRowCount = CRoomLoadingMgr::GetInstance()->GetRoomRowCount();
	const int iColCount = CRoomLoadingMgr::GetInstance()->GetRoomColCount();

	const float fLocalX = m_vPlayerPos.x - vCenter.x;
	const float fLocalZ = m_vPlayerPos.z - vCenter.z;

	const float fHalfGridX = (float)iColCount * vRoomSize.x * 0.5f;
	const float fHalfGridZ = (float)iRowCount * vRoomSize.z * 0.5f;

	const int iCol = (int)floorf((fLocalX + fHalfGridX) / vRoomSize.x);
	const int iRow = (int)floorf((fHalfGridZ - fLocalZ) / vRoomSize.z);

	if (0 > iCol || iCol >= iColCount || 0 > iRow || iRow >= iRowCount) return -1;

	return iRow * iColCount + iCol;
}

void CStage::ApplyRoomShader(EBiomeType eBiomeType)
{
    POST_EFFECT eEffect = POST_EFFECT::NONE;
    switch (eBiomeType)
    {
    case EBiomeType::AQUA: eEffect = POST_EFFECT::UNDERWATER; break;
    case EBiomeType::LAVA: eEffect = POST_EFFECT::LAVA; break;
    default: break;
    }
    CShaderEffectMgr::GetInstance()->Set_PostEffect(eEffect);
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
