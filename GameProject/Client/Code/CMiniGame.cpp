#include "pch.h"
#include "CMiniGame.h"
#include "CLayer.h"
#include "CDInputMgr.h"
#include "CManagement.h"
#include "CSkyBox.h"
#include "CMiniGameBG.h"
#include "CMG0_Player.h"
#include "CMG0_Enemy_Skull.h"
#include "CMG0_Bullet.h"
#include "CMG0_Deck.h"
#include "CMG0_Dancer.h"
#include "CMG0_HUD.h"
#include "CMG0_Intro.h"
#include "CMG0_Dialogue.h"
#include "CMG0_Train13.h"
#include "CMG0_Train14.h"
#include "CMG0_Herobrine.h"
#include "CMG0_Laser.h"
#include "CTexture.h"
#include "CGameMachine.h"

CMiniGame::CMiniGame(LPDIRECT3DDEVICE9 pGraphicDev)
    : CScene(pGraphicDev)
{
}

CMiniGame::~CMiniGame()
{
}

HRESULT CMiniGame::Ready_Scene()
{
    if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
        return E_FAIL;

    if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(L"UI_Layer")))
        return E_FAIL;

    Update_Camera();

    return S_OK;
}

_int CMiniGame::Update_Scene(_float fTimeDelta)
{
    m_fSceneTime += fTimeDelta;

    Update_BGM();

    if (m_pPlayer->Is_GameOver())
        m_fGameOverTimer -= fTimeDelta;
    else
    {
        m_fPlayTime += fTimeDelta;

        if (m_bEnemySpawn)
            Spawn_Enemy(fTimeDelta);

        Spawn_Train(fTimeDelta);
    }

    if (m_pDancer->Is_Boarded() && m_fSceneTime < MG_TRAIN_END_TIME)
        m_fCamRate = min(1.f, m_fCamRate + fTimeDelta / MG_TRAIN_CAM_TIME);
    else
        m_fCamRate = max(0.f, m_fCamRate - fTimeDelta / MG_TRAIN_CAM_TIME);

    _int iExit = CScene::Update_Scene(fTimeDelta);

    Update_Camera();

    if (m_pPlayer->Is_GameOver() && m_fGameOverTimer <= 0.f)
    {
        if (FAILED(CManagement::GetInstance()->Change_Scene(0, nullptr, true)))
            return -1;

        return iExit;
    }

    if (!m_bTimeOver && m_fSceneTime >= MG_BOSS_LIMIT_TIME && !m_pBoss->Is_Dying())
        m_bTimeOver = true;

    if (m_bTimeOver)
    {
        m_pPlayer->Set_Invincible(1.f);
        m_fWhiteTime += fTimeDelta;
        if (m_fWhiteTime >= MG_WHITE_EXIT_TIME)
        {
            if (FAILED(CManagement::GetInstance()->Change_Scene(0, nullptr, true)))
                return -1;

            return iExit;
        }
    }

    if (m_pBoss->Is_Dead() || m_fSceneTime >= MG_BGM_START_TIME + MG_BGM_LENGTH + MG_CLEAR_DELAY)
    {
        m_pPlayer->Set_Invincible(1.f);
        m_fClearTimer -= fTimeDelta;
        if (m_fClearTimer <= 0.f)
        {
            if (CGameMachine* pGameMachine = dynamic_cast<CGameMachine*>(CManagement::GetInstance()->Get_Scene(0)->Get_GameObject(L"Room_15_Layer", L"Room_15_ObjectTiling_71")))
            {
                pGameMachine->Set_Clear();
            }
            if (FAILED(CManagement::GetInstance()->Change_Scene(0, nullptr, true)))
                return -1;

            return iExit;
        }
    }

    if (CDInputMgr::GetInstance()->Key_Down(DIK_F2))
    {
        if (CGameMachine* pGameMachine = dynamic_cast<CGameMachine*>(CManagement::GetInstance()->Get_Scene(0)->Get_GameObject(L"Room_15_Layer", L"Room_15_ObjectTiling_71")))
        {
            pGameMachine->Set_Clear();
        }
        if (FAILED(CManagement::GetInstance()->Change_Scene(0, nullptr, true)))
            return -1;
    }

    return iExit;
}

void CMiniGame::LateUpdate_Scene(_float fTimeDelta)
{
    CScene::LateUpdate_Scene(fTimeDelta);
}

void CMiniGame::OnExit()
{
    CSoundMgr::GetInstance()->StopSFX(m_hBGM);
}

HRESULT CMiniGame::Add_GameObject(const wstring& pObjTag, CGameObject* pGameObject)
{
    CLayer* pLayer = Get_Layer(L"GameLogic_Layer");
    if (nullptr == pLayer)
        return E_FAIL;

    if (FAILED(pLayer->Add_GameObject(pObjTag, pGameObject)))
        return E_FAIL;

    return S_OK;
}

void CMiniGame::OnPlayerDead()
{
    m_fGameOverTimer = MG_GAMEOVER_DELAY;

    for (auto& Pair : Get_Layer(L"GameLogic_Layer")->Get_ObjMap())
    {
        CMG0_Bullet* pBullet = dynamic_cast<CMG0_Bullet*>(Pair.second);
        if (pBullet && MINIGAME0::BULLET_PLAYER == pBullet->Get_Type())
            pBullet->Set_Dead(true);
    }
}

void CMiniGame::Update_BGM()
{
    if (m_bBGMStarted || m_fSceneTime < MG_BGM_START_TIME)
        return;

    m_hBGM = CSoundMgr::GetInstance()->PlaySFX(L"MiniGame_0.wav");
    CSoundMgr::GetInstance()->SetSFXVolume(0.1f);
    m_bBGMStarted = true;
}

void CMiniGame::Spawn_Enemy(_float fTimeDelta)
{
    m_fSpawnTimer -= fTimeDelta;
    if (m_fSpawnTimer > 0.f)
        return;

    _vec3 vPos(MINIGAME0::Random(MG_FIELD_MIN_X, MG_FIELD_MAX_X), MINIGAME0::Random(-1.f, MG_FIELD_MAX_Y), MG_SPAWN_Z);
    _int iHp = MG_BASE_ATTACK * (3 + (_int)(m_fPlayTime * 0.25f));

    Add_GameObject(L"Enemy", CMG0_Enemy_Skull::Create(m_pGraphicDev, vPos, iHp));

    m_fSpawnTimer = max(0.4f, 1.2f - m_fPlayTime * 0.01f) * MINIGAME0::Random(0.7f, 1.3f);
}

void CMiniGame::Spawn_Train(_float fTimeDelta)
{
    if (!m_pDancer->Is_Boarded() || m_fSceneTime >= MG_TRAIN_END_TIME - MG_TRAIN_CLEAR_TIME)
        return;

    m_fTrainTime += fTimeDelta;

    _float fRate = min(1.f, m_fTrainTime / MG_TRAIN_RAMP_TIME);
    _float fSpeed = MG_TRAIN_SPEED_MIN + (MG_TRAIN_SPEED_MAX - MG_TRAIN_SPEED_MIN) * fRate;

    if (m_iTrain14Lane >= 0 && m_fTrainTime > m_fTrain14Arrive + MG_TRAIN14_SAFE_WINDOW)
        m_iTrain14Lane = -1;

    if (fRate >= MG_TRAIN14_START_RATE)
    {
        m_fTrain14Timer -= fTimeDelta;
        if (m_fTrain14Timer <= 0.f && m_iTrain14Lane < 0)
            Spawn_Train14(fRate, fSpeed);
    }

    m_fTrainTimer -= fTimeDelta;
    if (m_fTrainTimer > 0.f)
        return;

    _int iPrevSafe = m_iSafeLane;
    _int iStep = (fRate >= 0.5f && 0 == rand() % 4) ? 2 : 1;
    _int iSafeLane = m_iSafeLane + (rand() % 2 ? iStep : -iStep);

    if (iSafeLane < 0 || iSafeLane >= MG_LANE_COUNT)
        iSafeLane = 2 * m_iSafeLane - iSafeLane;

    m_iSafeLane = max(0, min(MG_LANE_COUNT - 1, iSafeLane));

    _float fArrive = m_fTrainTime + MG_SPAWN_Z / fSpeed;
    _bool bInTrain14 = (m_iSafeLane == m_iTrain14Lane || m_iSafeLane == m_iTrain14Lane + 1);

    if (m_iTrain14Lane >= 0 && bInTrain14 && fabsf(fArrive - m_fTrain14Arrive) < MG_TRAIN14_SAFE_WINDOW)
    {
        _int iBest = -1;

        for (_int i = 0; i < MG_LANE_COUNT; ++i)
        {
            if (i == m_iTrain14Lane || i == m_iTrain14Lane + 1)
                continue;

            if (iBest < 0 || abs(i - iPrevSafe) < abs(iBest - iPrevSafe) || (abs(i - iPrevSafe) == abs(iBest - iPrevSafe) && rand() % 2))
                iBest = i;
        }

        m_iSafeLane = iBest;
    }

    m_fTrainTimer = (MG_TRAIN_SPAWN_MAX - (MG_TRAIN_SPAWN_MAX - MG_TRAIN_SPAWN_MIN) * fRate) * MINIGAME0::Random(0.85f, 1.15f);

    _int iLane[MG_LANE_COUNT - 1];
    _int iLaneCount = 0;

    for (_int i = 0; i < MG_LANE_COUNT; ++i)
    {
        if (i != m_iSafeLane)
            iLane[iLaneCount++] = i;
    }

    for (_int i = iLaneCount; i > 1; --i)
        swap(iLane[i - 1], iLane[rand() % i]);

    _int iTrainCount = min(iLaneCount, 2 + (fRate >= 0.5f ? 1 : 0) + rand() % 2);

    for (_int i = 0; i < iTrainCount; ++i)
        Add_GameObject(L"Train", CMG0_Train13::Create(m_pGraphicDev, iLane[i], fSpeed));
}

void CMiniGame::Spawn_Train14(_float fRate, _float fSpeed)
{
    _float fSpeed14 = fSpeed * MG_TRAIN14_SPEED_RATE;
    _int iLeft = rand() % (MG_LANE_COUNT - 1);

    Add_GameObject(L"Train", CMG0_Train14::Create(m_pGraphicDev, iLeft, fSpeed14));

    m_iTrain14Lane = iLeft;
    m_fTrain14Arrive = m_fTrainTime + MG_TRAIN14_WARN_TIME + MG_SPAWN_Z / fSpeed14;
    m_fTrain14Timer = (MG_TRAIN14_INTERVAL_MAX - (MG_TRAIN14_INTERVAL_MAX - MG_TRAIN14_INTERVAL_MIN) * fRate) * MINIGAME0::Random(0.85f, 1.15f);
}

void CMiniGame::Clear_Enemy()
{
    for (auto& Pair : Get_Layer(L"GameLogic_Layer")->Get_ObjMap())
    {
        if (dynamic_cast<CMG0_Sprite*>(Pair.second) || dynamic_cast<CMG0_Laser*>(Pair.second))
            Pair.second->Set_Dead(true);
    }
}

_float CMiniGame::Get_WhiteRate()
{
    return min(1.f, m_fWhiteTime / MG_WHITE_FADE_TIME);
}

_uint CMiniGame::Get_TileFrame()
{
    if (m_fSceneTime >= MG_TRAIN_END_TIME)
        return MGBG_BOSS_TILE_FRAME;

    if (m_pDancer->Is_Boarded())
        return MGBG_TRAIN_TILE_FRAME;

    return MGBG_TILE_FRAME;
}

void CMiniGame::Update_Camera()
{
    const _vec3& vPlayerPos = m_pPlayer->Get_Pos();

    _float fCamRate = m_fCamRate * m_fCamRate * (3.f - 2.f * m_fCamRate);
    _float fEyeY = 2.8f + (MG_TRAIN_CAM_EYE_Y - 2.8f) * fCamRate;
    _float fEyeZ = -9.5f + (MG_TRAIN_CAM_EYE_Z + 9.5f) * fCamRate;
    _float fAtY = 0.6f + (MG_TRAIN_CAM_AT_Y - 0.6f) * fCamRate;

    _float fAtZ = MG_CAM_AT_Z + (MG_TRAIN_CAM_AT_Z - MG_CAM_AT_Z) * fCamRate;
    _float fFov = MG_CAM_FOV + (MG_TRAIN_CAM_FOV - MG_CAM_FOV) * fCamRate;

    _vec3 vEye(vPlayerPos.x * 0.55f, vPlayerPos.y + fEyeY, fEyeZ);
    _vec3 vAt(vPlayerPos.x * 0.75f, vPlayerPos.y + fAtY, fAtZ);
    _vec3 vUp(0.f, 1.f, 0.f);

    _matrix matView, matProj;
    D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
    D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(fFov), (_float)WINCX / WINCY, 0.1f, 300.f);

    m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
    m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);
}

HRESULT CMiniGame::Ready_Environment_Layer(const _tchar* pLayerTag)
{
    CLayer* pLayer = CLayer::Create();
    if (nullptr == pLayer)
        return E_FAIL;

    m_mapLayer.insert({ pLayerTag, pLayer });

    CGameObject* pGameObject = nullptr;

    pGameObject = CSkyBox::Create(m_pGraphicDev);
    if (nullptr == pGameObject)
        return E_FAIL;

    if (FAILED(pLayer->Add_GameObject(L"SkyBox", pGameObject)))
        return E_FAIL;

    for (_int i = MGBG_TILE_MIN_X; i <= MGBG_TILE_MAX_X; i++)
    {
        for (_int j = MGBG_TILE_MIN_Z; j <= MGBG_TILE_MAX_Z; j++)
        {
            pGameObject = CMiniGameBG::Create(m_pGraphicDev, { i, j });
            if (nullptr == pGameObject)
                return E_FAIL;

            if (FAILED(pLayer->Add_GameObject((L"Tile_" + to_wstring(i) + L"_" + to_wstring(j)), pGameObject)))
                return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT CMiniGame::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
    CLayer* pLayer = CLayer::Create();
    if (nullptr == pLayer)
        return E_FAIL;

    m_mapLayer.insert({ pLayerTag, pLayer });

    m_pPlayer = CMG0_Player::Create(m_pGraphicDev);
    if (nullptr == m_pPlayer)
        return E_FAIL;

    if (FAILED(pLayer->Add_GameObject(L"Player", m_pPlayer)))
        return E_FAIL;

    m_pDeck = CMG0_Deck::Create(m_pGraphicDev);
    if (nullptr == m_pDeck)
        return E_FAIL;

    if (FAILED(pLayer->Add_GameObject(L"Deck", m_pDeck)))
        return E_FAIL;

    m_pDancer = CMG0_Dancer::Create(m_pGraphicDev);
    if (nullptr == m_pDancer)
        return E_FAIL;

    if (FAILED(pLayer->Add_GameObject(L"Dancer", m_pDancer)))
        return E_FAIL;

    m_pBoss = CMG0_Herobrine::Create(m_pGraphicDev);
    if (nullptr == m_pBoss)
        return E_FAIL;

    if (FAILED(pLayer->Add_GameObject(L"Boss", m_pBoss)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMiniGame::Ready_UI_Layer(const _tchar* pLayerTag)
{
    CLayer* pLayer = CLayer::Create();
    if (nullptr == pLayer)
        return E_FAIL;

    m_mapLayer.insert({ pLayerTag, pLayer });

    CGameObject* pGameObject = CMG0_HUD::Create(m_pGraphicDev);
    if (nullptr == pGameObject)
        return E_FAIL;

    if (FAILED(pLayer->Add_GameObject(L"HUD", pGameObject)))
        return E_FAIL;

    pGameObject = CMG0_Intro::Create(m_pGraphicDev);
    if (nullptr == pGameObject)
        return E_FAIL;

    if (FAILED(pLayer->Add_GameObject(L"Intro", pGameObject)))
        return E_FAIL;

    m_pDialogue = CMG0_Dialogue::Create(m_pGraphicDev);
    if (nullptr == m_pDialogue)
        return E_FAIL;

    if (FAILED(pLayer->Add_GameObject(L"Dialogue", m_pDialogue)))
        return E_FAIL;

    return S_OK;
}

CMiniGame* CMiniGame::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMiniGame* pMiniGame = new CMiniGame(pGraphicDev);
    if (FAILED(pMiniGame->Ready_Scene()))
    {
        Safe_Release(pMiniGame);
        MSG_BOX("CMiniGame Create Failed");
        return nullptr;
    }

    return pMiniGame;
}

void CMiniGame::Free()
{
    CScene::Free();
}
