#include "pch.h"
#include "CMG0_Player.h"
#include "CManagement.h"
#include "CDInputMgr.h"
#include "CRenderer.h"
#include "CMiniGame.h"
#include "CMG0_Deck.h"
#include "CMG0_Dancer.h"
#include "CMG0_Bullet.h"
#include "CMG0_Explosion.h"

CMG0_Player::CMG0_Player(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CMG0_Player::~CMG0_Player()
{
}

HRESULT CMG0_Player::Ready_GameObject()
{
    Ready_Mesh();
    Reset();

    return CGameObject::Ready_GameObject();
}

_int CMG0_Player::Update_GameObject(_float fTimeDelta)
{
    CMiniGame* pScene = dynamic_cast<CMiniGame*>(CManagement::GetInstance()->GetCurrentScene());
    _bool bActive = pScene && pScene->Get_Dancer()->Is_Boarded();

    if (bActive && !m_bActive)
        m_fInvincible = MG_START_INVINCIBLE;

    m_bActive = bActive;
    m_bVisible = pScene && pScene->Get_Deck()->Is_Finished();

    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    if (!m_bVisible)
        return iExit;

    if (m_bActive && !m_bGameOver)
    {
        if (m_fInvincible > 0.f)
            m_fInvincible -= fTimeDelta;

        if (pScene->Get_SceneTime() < MG_TRAIN_END_TIME)
        {
            Move_Train(fTimeDelta);
        }
        else
        {
            Move_Free(fTimeDelta);
            Fire(fTimeDelta);
        }
    }

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CMG0_Player::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMG0_Player::Render_GameObject()
{
    _bool bBlink = m_bActive && (m_fInvincible > 0.f) && (((_int)(m_fInvincible * 12.f)) % 2 == 0);
    if (m_bGameOver || bBlink)
        return;

    DWORD dwCull, dwLighting, dwColorOp, dwColorArg1, dwAlphaOp, dwAlphaArg1;
    m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &dwCull);
    m_pGraphicDev->GetRenderState(D3DRS_LIGHTING, &dwLighting);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwColorArg1);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAlphaOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAlphaArg1);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetTexture(0, nullptr);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

    _matrix matRotZ, matRotX, matTrans;
    D3DXMatrixRotationZ(&matRotZ, m_fRoll);
    D3DXMatrixRotationX(&matRotX, m_fPitch);
    D3DXMatrixTranslation(&matTrans, m_vPos.x, m_vPos.y, m_vPos.z);
    _matrix matWorld = matRotZ * matRotX * matTrans;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
    m_pGraphicDev->SetFVF(MG_FVF);
    m_pGraphicDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, (UINT)(m_vecMesh.size() / 3), m_vecMesh.data(), sizeof(MINIGAME0::MGVTX));

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwAlphaArg1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwAlphaOp);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwColorArg1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, dwColorOp);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, dwLighting);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, dwCull);
}

void CMG0_Player::OnHit()
{
    if (m_bGameOver || !m_bActive || m_fInvincible > 0.f)
        return;

    --m_iLife;
    m_fInvincible = MG_HIT_INVINCIBLE;
    Effect_Explode(m_vPos, 1.2f);

    if (m_iLife <= 0)
    {
        m_iLife = 0;
        m_bGameOver = true;

        Effect_Explode(m_vPos, 3.f);
        Effect_Explode(m_vPos + _vec3(1.5f, 0.5f, 0.f), 2.f);
        Effect_Explode(m_vPos + _vec3(-1.5f, -0.5f, 0.f), 2.f);

        CMiniGame* pScene = dynamic_cast<CMiniGame*>(CManagement::GetInstance()->GetCurrentScene());
        if (pScene)
            pScene->OnPlayerDead();
    }
}

void CMG0_Player::Reset()
{
    m_vPos = MG_PLAYER_CENTER;
    m_fRoll = 0.f;
    m_fPitch = 0.f;
    m_iLane = MG_LANE_COUNT / 2;
    m_fInvincible = MG_START_INVINCIBLE;
    m_iLife = MG_MAX_LIFE;
    m_bGameOver = false;
}

void CMG0_Player::Ready_Mesh()
{
    _vec3 vTip(0.f, 0.f, 1.6f);
    _vec3 vLeft(-1.2f, 0.f, -1.2f);
    _vec3 vRight(1.2f, 0.f, -1.2f);
    _vec3 vNotch(0.f, 0.3f, -0.5f);

    m_vecMesh.push_back({ vTip, 0xFF4FA0FF });
    m_vecMesh.push_back({ vLeft, 0xFF4FA0FF });
    m_vecMesh.push_back({ vNotch, 0xFF4FA0FF });

    m_vecMesh.push_back({ vTip, 0xFF1F5FC0 });
    m_vecMesh.push_back({ vNotch, 0xFF1F5FC0 });
    m_vecMesh.push_back({ vRight, 0xFF1F5FC0 });
}

void CMG0_Player::Move_Free(_float fTimeDelta)
{
    CDInputMgr* pInput = CDInputMgr::GetInstance();

    _float fInputX = 0.f;
    _float fInputY = 0.f;

    if (pInput->Key_Press(DIK_LEFT))
        fInputX -= 1.f;
    if (pInput->Key_Press(DIK_RIGHT))
        fInputX += 1.f;
    if (pInput->Key_Press(DIK_UP))
        fInputY += 1.f;
    if (pInput->Key_Press(DIK_DOWN))
        fInputY -= 1.f;

    m_vPos.x += fInputX * m_fSpeed * fTimeDelta;
    m_vPos.y += fInputY * m_fSpeed * fTimeDelta;
    m_vPos.x = max(MG_FIELD_MIN_X, min(MG_FIELD_MAX_X, m_vPos.x));
    m_vPos.y = min(MG_FIELD_MAX_Y, m_vPos.y);

    if (m_vPos.y < MG_FIELD_MIN_Y)
        m_vPos.y += (MG_FIELD_MIN_Y - m_vPos.y) * min(1.f, 5.f * fTimeDelta);

    _float fTargetRoll = D3DXToRadian(-40.f) * fInputX;
    _float fTargetPitch = D3DXToRadian(-15.f) * fInputY;
    _float fLerp = min(1.f, 8.f * fTimeDelta);
    m_fRoll += (fTargetRoll - m_fRoll) * fLerp;
    m_fPitch += (fTargetPitch - m_fPitch) * fLerp;
}

void CMG0_Player::Fire(_float fTimeDelta)
{
    m_fFireCool -= fTimeDelta;
    if (!CDInputMgr::GetInstance()->Key_Press(DIK_Z) || m_fFireCool > 0.f)
        return;

    CMiniGame* pScene = dynamic_cast<CMiniGame*>(CManagement::GetInstance()->GetCurrentScene());
    if (nullptr == pScene)
        return;

    m_fFireCool = m_fFireInterval;

    for (_int i = 0; i < m_iBulletCount; ++i)
    {
        _vec3 vPos = m_vPos + _vec3((i - (m_iBulletCount - 1) * 0.5f) * 0.6f, 0.f, 1.8f);

        pScene->Add_GameObject(L"Bullet",
            CMG0_Bullet::Create(m_pGraphicDev, MINIGAME0::BULLET_PLAYER, vPos, _vec3(0.f, 0.f, 75.f), m_iAttack));
    }
}

void CMG0_Player::Move_Train(_float fTimeDelta)
{
    CDInputMgr* pInput = CDInputMgr::GetInstance();

    if (pInput->Key_Down(DIK_LEFT))
        Move_Lane(-1);
    if (pInput->Key_Down(DIK_RIGHT))
        Move_Lane(1);

    if (0 != m_iLaneDir)
    {
        if (!pInput->Key_Press(m_iLaneDir < 0 ? DIK_LEFT : DIK_RIGHT))
        {
            m_iLaneDir = 0;
        }
        else
        {
            m_fLaneTime -= fTimeDelta;

            if (m_fLaneTime <= 0.f)
            {
                Move_Lane(m_iLaneDir);
                m_fLaneTime = MG_LANE_REPEAT_TIME;
            }
        }
    }

    _float fTargetX = MINIGAME0::Get_LaneX(m_iLane);
    _float fMoveRate = max(-1.f, min(1.f, (fTargetX - m_vPos.x) / MG_LANE_GAP));

    m_vPos.x += (fTargetX - m_vPos.x) * min(1.f, MG_LANE_MOVE_SPEED * fTimeDelta);
    m_vPos.y += (MG_TRAIN_PLAYER_Y - m_vPos.y) * min(1.f, 3.f * fTimeDelta);

    _float fTargetRoll = D3DXToRadian(-40.f) * fMoveRate;
    _float fLerp = min(1.f, 8.f * fTimeDelta);
    m_fRoll += (fTargetRoll - m_fRoll) * fLerp;
    m_fPitch += (0.f - m_fPitch) * fLerp;
}

void CMG0_Player::Move_Lane(_int iDir)
{
    m_iLane = max(0, min(MG_LANE_COUNT - 1, m_iLane + iDir));
    m_iLaneDir = iDir;
    m_fLaneTime = MG_LANE_REPEAT_DELAY;
}

void CMG0_Player::Effect_Explode(const _vec3& vPos, _float fScale)
{
    CMiniGame* pScene = dynamic_cast<CMiniGame*>(CManagement::GetInstance()->GetCurrentScene());
    if (nullptr == pScene)
        return;

    pScene->Add_GameObject(L"Explosion", CMG0_Explosion::Create(m_pGraphicDev, vPos, fScale));
}

CMG0_Player* CMG0_Player::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMG0_Player* pPlayer = new CMG0_Player(pGraphicDev);

    if (FAILED(pPlayer->Ready_GameObject()))
    {
        Safe_Release(pPlayer);
        MSG_BOX("CMG0_Player Create Failed");
        return nullptr;
    }

    return pPlayer;
}

void CMG0_Player::Free()
{
    CGameObject::Free();
}
