#include "pch.h"
#include "CMiniGame.h"
#include "CLayer.h"
#include "CProtoMgr.h"
#include "CDInputMgr.h"
#include "CManagement.h"
#include "CSkyBox.h"
#include "CMiniGameBG.h"
#include "CFontMgr.h"

static const _int   MG_MAX_LIFE = 5;
static const _uint  MG_HEART_FULL = 4;
static const _float MG_START_INVINCIBLE = 1.6f;
static const DWORD  MG_FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
static const _int   MG_GATE_TYPE_COUNT = 3;
static const _float MG_GATE_HALF_X = 4.5f;
static const _float MG_GATE_HALF_Y = 4.25f;
static const _float MG_GATE_CENTER_Y = 2.75f;
static const _tchar* MG_GATE_TEXT[MG_GATE_TYPE_COUNT] = { L"공격력 x2", L"총알 +1", L"연사 UP" };
static const DWORD  MG_GATE_COLOR[MG_GATE_TYPE_COUNT] = { D3DCOLOR_ARGB(110, 255, 90, 60), D3DCOLOR_ARGB(110, 60, 140, 255), D3DCOLOR_ARGB(110, 60, 220, 110) };
static const _int   MG_NEEDED_KILL_COUNT = 10;

static _float MG_Random(_float fMin, _float fMax)
{
    return fMin + (fMax - fMin) * (rand() / (_float)RAND_MAX);
}

static DWORD MG_FloatToDword(_float fValue)
{
    return *reinterpret_cast<DWORD*>(&fValue);
}

CMiniGame::CMiniGame(LPDIRECT3DDEVICE9 pGraphicDev)
    : CScene(pGraphicDev)
{
}

CMiniGame::~CMiniGame()
{
}

HRESULT CMiniGame::Ready_Scene()
{
    if (FAILED(Ready_Resource()))
        return E_FAIL;

    if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
        return E_FAIL;

    if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer(L"UI_Layer")))
        return E_FAIL;

    Build_PlaneMesh();

    Reset_Game();
    Update_Camera();

    return S_OK;
}

HRESULT CMiniGame::Ready_Resource()
{
    CProtoMgr* pProtoMgr = CProtoMgr::GetInstance();

    m_pBufferCom = dynamic_cast<CRcTex*>(pProtoMgr->Clone_Prototype(L"Proto_RcTex"));
    m_pEnemyTex = dynamic_cast<CTexture*>(pProtoMgr->Clone_Prototype(L"Proto_skull3Texture"));
    m_pEnemyBulletTex = dynamic_cast<CTexture*>(pProtoMgr->Clone_Prototype(L"Proto_fireballTexture"));
    m_pBulletTex = dynamic_cast<CTexture*>(pProtoMgr->Clone_Prototype(L"Proto_Bullet_Default_Texture"));
    m_pExplodeTex = dynamic_cast<CTexture*>(pProtoMgr->Clone_Prototype(L"Proto_smallexplodeTexture"));
    m_pHeartTex = dynamic_cast<CTexture*>(pProtoMgr->Clone_Prototype(L"Proto_HpUITexture"));
    m_pHitTex = dynamic_cast<CTexture*>(pProtoMgr->Clone_Prototype(L"Proto_HitScreenTexture"));

    if (nullptr == m_pBufferCom || nullptr == m_pEnemyTex || nullptr == m_pEnemyBulletTex ||
        nullptr == m_pBulletTex || nullptr == m_pExplodeTex || nullptr == m_pHeartTex || nullptr == m_pHitTex)
        return E_FAIL;

    return S_OK;
}

_int CMiniGame::Update_Scene(_float fTimeDelta)
{
    m_fAnimTime += fTimeDelta;

    if (m_bGameOver)
    {
        m_fRestartTimer -= fTimeDelta;
        if (m_fRestartTimer <= 0.f)
            Reset_Game();
    }
    else
    {
        m_fPlayTime += fTimeDelta;
        Update_Player(fTimeDelta);
    }

    Update_Bullets(fTimeDelta);
    Update_Enemies(fTimeDelta);
    Update_Gates(fTimeDelta);
    Update_Explosions(fTimeDelta);
    Update_Camera();

    _int iExit = CScene::Update_Scene(fTimeDelta);

    if (m_iKillCount >= MG_NEEDED_KILL_COUNT)
    {
        m_fInvincible = 1.f;
        m_fClearTimer -= fTimeDelta;
        if (m_fClearTimer <= 0.f)
        {
            m_fClearTimer = 0.f;
            if (FAILED(CManagement::GetInstance()->Change_Scene(0, nullptr, true)))
                return -1;

            return iExit;
        }
    }

    if (CDInputMgr::GetInstance()->Key_Down(DIK_F2))
    {
        if (FAILED(CManagement::GetInstance()->Change_Scene(0, nullptr, true)))
            return -1;

        return iExit;
    }

    return iExit;
}

void CMiniGame::LateUpdate_Scene(_float fTimeDelta)
{
    CScene::LateUpdate_Scene(fTimeDelta);
}

void CMiniGame::Reset_Game()
{
    m_vecBullets.clear();
    m_vecEnemies.clear();
    m_vecEnemyBullets.clear();
    m_vecExplosions.clear();
    m_vecGates.clear();

    m_fGateTimer = 4.f;
    m_fFireInterval = 0.12f;
    m_iDamage = 1;
    m_iBulletCount = 1;

    m_vPlayerPos = { 0.f, 2.f, 0.f };
    m_fRoll = 0.f;
    m_fPitch = 0.f;
    m_fFireCool = 0.f;
    m_fSpawnTimer = 1.5f;
    m_fPlayTime = 0.f;
    m_fInvincible = MG_START_INVINCIBLE;
    m_fRestartTimer = 0.f;
    m_fClearTimer = 3.f;
    m_iLife = MG_MAX_LIFE;
    m_bGameOver = false;
    m_iKillCount = 0;
}

void CMiniGame::Update_Player(_float fTimeDelta)
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

    const _float fSpeed = 11.f;
    m_vPlayerPos.x += fInputX * fSpeed * fTimeDelta;
    m_vPlayerPos.y += fInputY * fSpeed * fTimeDelta;
    m_vPlayerPos.x = max(-10.f, min(10.f, m_vPlayerPos.x));
    m_vPlayerPos.y = max(-1.5f, min(7.f, m_vPlayerPos.y));

    _float fTargetRoll = D3DXToRadian(-40.f) * fInputX;
    _float fTargetPitch = D3DXToRadian(-15.f) * fInputY;
    _float fLerp = min(1.f, 8.f * fTimeDelta);
    m_fRoll += (fTargetRoll - m_fRoll) * fLerp;
    m_fPitch += (fTargetPitch - m_fPitch) * fLerp;

    if (m_fInvincible > 0.f)
        m_fInvincible -= fTimeDelta;

    m_fFireCool -= fTimeDelta;
    if (pInput->Key_Press(DIK_Z) && m_fFireCool <= 0.f)
    {
        m_fFireCool = m_fFireInterval;

        for (_int i = 0; i < m_iBulletCount; ++i)
        {
            MGOBJ tBullet;
            tBullet.vPos = m_vPlayerPos + _vec3((i - (m_iBulletCount - 1) * 0.5f) * 0.6f, 0.f, 1.8f);
            tBullet.vVel = { 0.f, 0.f, 75.f };
            tBullet.fTimer = 2.f;
            tBullet.fScale = 0.3f;
            tBullet.iHp = m_iDamage;
            m_vecBullets.push_back(tBullet);
        }
    }
}

void CMiniGame::Update_Bullets(_float fTimeDelta)
{
    for (auto iter = m_vecBullets.begin(); iter != m_vecBullets.end(); )
    {
        iter->vPos += iter->vVel * fTimeDelta;
        iter->fTimer -= fTimeDelta;

        _bool bRemove = (iter->fTimer <= 0.f || iter->vPos.z > 120.f);

        if (!bRemove)
        {
            for (auto& tEnemy : m_vecEnemies)
            {
                if (tEnemy.iHp <= 0)
                    continue;

                _vec3 vDiff = tEnemy.vPos - iter->vPos;
                _float fRadius = 1.1f * tEnemy.fScale;
                if (D3DXVec3LengthSq(&vDiff) < fRadius * fRadius)
                {
                    tEnemy.iHp -= iter->iHp;
                    if (tEnemy.iHp <= 0)
                        ++m_iKillCount;
                    tEnemy.fFlash = 0.08f;
                    Spawn_Explosion(iter->vPos, 0.5f);
                    bRemove = true;
                    break;
                }
            }
        }

        if (bRemove)
            iter = m_vecBullets.erase(iter);
        else
            ++iter;
    }

    for (auto iter = m_vecEnemyBullets.begin(); iter != m_vecEnemyBullets.end(); )
    {
        iter->vPos += iter->vVel * fTimeDelta;
        iter->fTimer -= fTimeDelta;

        _bool bRemove = (iter->fTimer <= 0.f || iter->vPos.z < -15.f);

        if (!bRemove && !m_bGameOver)
        {
            _vec3 vDiff = m_vPlayerPos - iter->vPos;
            if (D3DXVec3LengthSq(&vDiff) < 0.9f * 0.9f)
            {
                Spawn_Explosion(iter->vPos, 0.8f);
                Hit_Player();
                bRemove = true;
            }
        }

        if (bRemove)
            iter = m_vecEnemyBullets.erase(iter);
        else
            ++iter;
    }
}

void CMiniGame::Update_Enemies(_float fTimeDelta)
{
    if (!m_bGameOver)
    {
        m_fSpawnTimer -= fTimeDelta;
        if (m_fSpawnTimer <= 0.f)
        {
            MGOBJ tEnemy;
            tEnemy.vPos = { MG_Random(-10.f, 10.f), MG_Random(-1.f, 7.f), 115.f };
            tEnemy.vVel = { 0.f, 0.f, -MG_Random(16.f, 22.f) };
            tEnemy.fPhase = MG_Random(0.f, D3DX_PI * 2.f);
            tEnemy.fCool = MG_Random(1.f, 2.5f);
            tEnemy.fScale = 1.2f;
            tEnemy.iHp = 3 + (_int)(m_fPlayTime * 0.25f);
            m_vecEnemies.push_back(tEnemy);

            m_fSpawnTimer = max(0.4f, 1.2f - m_fPlayTime * 0.01f) * MG_Random(0.7f, 1.3f);
        }
    }

    for (auto iter = m_vecEnemies.begin(); iter != m_vecEnemies.end(); )
    {
        MGOBJ& tEnemy = *iter;

        if (tEnemy.iHp <= 0)
        {
            Spawn_Explosion(tEnemy.vPos, 2.f);
            iter = m_vecEnemies.erase(iter);
            continue;
        }

        tEnemy.fTimer += fTimeDelta;
        if (tEnemy.fFlash > 0.f)
            tEnemy.fFlash -= fTimeDelta;

        tEnemy.vPos.x += cosf(tEnemy.fTimer * 2.f + tEnemy.fPhase) * 4.f * fTimeDelta;
        tEnemy.vPos.y += sinf(tEnemy.fTimer * 1.5f + tEnemy.fPhase) * 2.f * fTimeDelta;
        tEnemy.vPos += tEnemy.vVel * fTimeDelta;

        tEnemy.fCool -= fTimeDelta;
        if (!m_bGameOver && tEnemy.fCool <= 0.f && tEnemy.vPos.z > 20.f && tEnemy.vPos.z < 95.f)
        {
            _vec3 vDir = m_vPlayerPos - tEnemy.vPos;
            D3DXVec3Normalize(&vDir, &vDir);

            MGOBJ tShot;
            tShot.vPos = tEnemy.vPos;
            tShot.vVel = vDir * 24.f;
            tShot.fTimer = 6.f;
            tShot.fScale = 0.5f;
            m_vecEnemyBullets.push_back(tShot);

            tEnemy.fCool = MG_Random(2.f, 3.5f);
        }

        if (!m_bGameOver)
        {
            _vec3 vDiff = m_vPlayerPos - tEnemy.vPos;
            _float fRadius = 1.4f * tEnemy.fScale;
            if (D3DXVec3LengthSq(&vDiff) < fRadius * fRadius)
            {
                tEnemy.iHp = 0;
                Hit_Player();
                ++iter;
                continue;
            }
        }

        if (tEnemy.vPos.z < -15.f)
            iter = m_vecEnemies.erase(iter);
        else
            ++iter;
    }
}

void CMiniGame::Update_Explosions(_float fTimeDelta)
{
    for (auto iter = m_vecExplosions.begin(); iter != m_vecExplosions.end(); )
    {
        iter->fTimer -= fTimeDelta;
        if (iter->fTimer <= 0.f)
        {
            iter = m_vecExplosions.erase(iter);
            continue;
        }

        iter->vPos += iter->vVel * fTimeDelta;
        ++iter;
    }
}

void CMiniGame::Spawn_Explosion(const _vec3& vPos, _float fScale)
{
    MGOBJ tExplosion;
    tExplosion.vPos = vPos;
    tExplosion.vVel = { 0.f, 0.f, -CMiniGameBG::SCROLL_SPEED * 0.3f };
    tExplosion.fTimer = tExplosion.fLife = 0.4f;
    tExplosion.fScale = fScale;
    m_vecExplosions.push_back(tExplosion);
}

void CMiniGame::Hit_Player()
{
    if (m_bGameOver || m_fInvincible > 0.f)
        return;

    --m_iLife;
    m_fInvincible = 2.f;
    Spawn_Explosion(m_vPlayerPos, 1.2f);

    if (m_iLife <= 0)
    {
        m_iLife = 0;
        m_bGameOver = true;
        m_fRestartTimer = 3.f;
        m_vecBullets.clear();
        Spawn_Explosion(m_vPlayerPos, 3.f);
        Spawn_Explosion(m_vPlayerPos + _vec3(1.5f, 0.5f, 0.f), 2.f);
        Spawn_Explosion(m_vPlayerPos + _vec3(-1.5f, -0.5f, 0.f), 2.f);
    }
}

void CMiniGame::Update_Camera()
{
    _vec3 vEye(m_vPlayerPos.x * 0.55f, m_vPlayerPos.y + 2.8f, -9.5f);
    _vec3 vAt(m_vPlayerPos.x * 0.75f, m_vPlayerPos.y + 0.6f, 14.f);
    _vec3 vUp(0.f, 1.f, 0.f);

    D3DXMatrixLookAtLH(&m_matView, &vEye, &vAt, &vUp);
    D3DXMatrixPerspectiveFovLH(&m_matProj, D3DXToRadian(60.f), (_float)WINCX / WINCY, 0.1f, 300.f);

    D3DXMatrixInverse(&m_matBill, nullptr, &m_matView);
    m_matBill._41 = 0.f;
    m_matBill._42 = 0.f;
    m_matBill._43 = 0.f;

    m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);
    m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);
}

void CMiniGame::Render_Scene()
{
    DWORD dwLighting, dwCull, dwZEnable, dwZWrite, dwFog, dwTexFactor, dwAlphaBlend, dwSrcBlend, dwDestBlend;
    DWORD dwAlphaTest, dwAlphaRef, dwAlphaFunc, dwMagFilter, dwMinFilter;
    DWORD dwColorOp, dwColorArg1, dwColorArg2, dwAlphaOp, dwAlphaArg1, dwAlphaArg2;

    m_pGraphicDev->GetRenderState(D3DRS_LIGHTING, &dwLighting);
    m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &dwCull);
    m_pGraphicDev->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
    m_pGraphicDev->GetRenderState(D3DRS_ZWRITEENABLE, &dwZWrite);
    m_pGraphicDev->GetRenderState(D3DRS_FOGENABLE, &dwFog);
    m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &dwTexFactor);
    m_pGraphicDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlphaBlend);
    m_pGraphicDev->GetRenderState(D3DRS_SRCBLEND, &dwSrcBlend);
    m_pGraphicDev->GetRenderState(D3DRS_DESTBLEND, &dwDestBlend);
    m_pGraphicDev->GetRenderState(D3DRS_ALPHATESTENABLE, &dwAlphaTest);
    m_pGraphicDev->GetRenderState(D3DRS_ALPHAREF, &dwAlphaRef);
    m_pGraphicDev->GetRenderState(D3DRS_ALPHAFUNC, &dwAlphaFunc);
    m_pGraphicDev->GetSamplerState(0, D3DSAMP_MAGFILTER, &dwMagFilter);
    m_pGraphicDev->GetSamplerState(0, D3DSAMP_MINFILTER, &dwMinFilter);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwColorArg1);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG2, &dwColorArg2);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAlphaOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAlphaArg1);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG2, &dwAlphaArg2);

    m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);
    m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_FOGENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
    m_pGraphicDev->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_ARGB(255, 16, 24, 48));
    m_pGraphicDev->SetRenderState(D3DRS_FOGSTART, MG_FloatToDword(45.f));
    m_pGraphicDev->SetRenderState(D3DRS_FOGEND, MG_FloatToDword(110.f));
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);

    _bool bBlink = (m_fInvincible > 0.f) && (((_int)(m_fInvincible * 12.f)) % 2 == 0);
    if (!m_bGameOver && !bBlink && !m_vecPlaneMesh.empty())
    {
        m_pGraphicDev->SetTexture(0, nullptr);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

        _matrix matRotZ, matRotX, matTrans;
        D3DXMatrixRotationZ(&matRotZ, m_fRoll);
        D3DXMatrixRotationX(&matRotX, m_fPitch);
        D3DXMatrixTranslation(&matTrans, m_vPlayerPos.x, m_vPlayerPos.y, m_vPlayerPos.z);
        _matrix matWorld = matRotZ * matRotX * matTrans;

        m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
        m_pGraphicDev->SetFVF(MG_FVF);
        m_pGraphicDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, (UINT)(m_vecPlaneMesh.size() / 3), m_vecPlaneMesh.data(), sizeof(MGVTX));
    }

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x80);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

    for (auto& tEnemy : m_vecEnemies)
        Render_Sprite(m_pEnemyTex, 0, tEnemy.vPos, tEnemy.fScale, tEnemy.fFlash > 0.f ? 0xFFFF6060 : 0xFFFFFFFF);

    _uint iFireFrame = (_uint)(m_fAnimTime * 12.f) % (_uint)max(1, m_pEnemyBulletTex->GetCount());
    for (auto& tShot : m_vecEnemyBullets)
        Render_Sprite(m_pEnemyBulletTex, iFireFrame, tShot.vPos, tShot.fScale, 0xFFFFFFFF);

    _uint iBulletFrame = (_uint)(m_fAnimTime * 15.f) % (_uint)max(1, m_pBulletTex->GetCount());
    for (auto& tBullet : m_vecBullets)
        Render_Sprite(m_pBulletTex, iBulletFrame, tBullet.vPos, tBullet.fScale, 0xFFFFFFFF);

    _int iExplodeCount = max(1, m_pExplodeTex->GetCount());
    for (auto& tExplosion : m_vecExplosions)
    {
        _int iFrame = (_int)((1.f - tExplosion.fTimer / tExplosion.fLife) * iExplodeCount);
        iFrame = max(0, min(iExplodeCount - 1, iFrame));
        Render_Sprite(m_pExplodeTex, (_uint)iFrame, tExplosion.vPos, tExplosion.fScale, 0xFFFFFFFF);
    }

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    Render_Gates();

    m_pGraphicDev->SetRenderState(D3DRS_FOGENABLE, FALSE);

    Render_HUD();

    m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);
    m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, dwLighting);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, dwCull);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, dwZEnable);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, dwZWrite);
    m_pGraphicDev->SetRenderState(D3DRS_FOGENABLE, dwFog);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwTexFactor);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlphaBlend);
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, dwSrcBlend);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, dwDestBlend);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, dwAlphaTest);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, dwAlphaRef);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, dwAlphaFunc);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, dwMagFilter);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, dwMinFilter);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, dwColorOp);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwColorArg1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, dwColorArg2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwAlphaOp);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwAlphaArg1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, dwAlphaArg2);

    Render_GateText();
}

void CMiniGame::Render_Sprite(CTexture* pTexture, _uint iFrame, const _vec3& vPos, _float fScale, DWORD dwTint)
{
    _matrix matScale, matTrans;
    D3DXMatrixScaling(&matScale, fScale, fScale, fScale);
    D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);
    _matrix matWorld = matScale * m_matBill * matTrans;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwTint);
    pTexture->Set_Texture(iFrame);
    m_pBufferCom->Render_Buffer();
}

void CMiniGame::Render_ScreenQuad(CTexture* pTexture, _uint iFrame, _float fX, _float fY, _float fSizeX, _float fSizeY, DWORD dwTint)
{
    _matrix matScale, matTrans;
    D3DXMatrixScaling(&matScale, fSizeX, fSizeY, 1.f);
    D3DXMatrixTranslation(&matTrans, fX, fY, 0.5f);
    _matrix matWorld = matScale * matTrans;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwTint);
    pTexture->Set_Texture(iFrame);
    m_pBufferCom->Render_Buffer();
}

void CMiniGame::Render_HUD()
{
    _matrix matIdentity, matOrtho;
    D3DXMatrixIdentity(&matIdentity);
    D3DXMatrixOrthoLH(&matOrtho, (_float)WINCX, (_float)WINCY, 0.f, 1.f);

    m_pGraphicDev->SetTransform(D3DTS_VIEW, &matIdentity);
    m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOrtho);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    if (m_fInvincible > 1.6f && !m_bGameOver)
    {
        _int iAlpha = (_int)((m_fInvincible - 1.6f) / 0.4f * 255.f);
        iAlpha = max(0, min(255, iAlpha));
        Render_ScreenQuad(m_pHitTex, 0, 0.f, 0.f, WINCX * 0.5f, WINCY * 0.5f, D3DCOLOR_ARGB(iAlpha, 255, 255, 255));
    }

    _uint iFullHeart = MG_HEART_FULL;
    for (_int i = 0; i < MG_MAX_LIFE; ++i)
    {
        _float fX = -WINCX * 0.5f + 34.f + i * 40.f;
        _float fY = WINCY * 0.5f - 34.f;
        Render_ScreenQuad(m_pHeartTex, i < m_iLife ? iFullHeart : 0, fX, fY, 16.f, 16.f, 0xFFFFFFFF);
    }

    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
}

void CMiniGame::Build_PlaneMesh()
{
    _vec3 vTip(0.f, 0.f, 1.6f);
    _vec3 vLeft(-1.2f, 0.f, -1.2f);
    _vec3 vRight(1.2f, 0.f, -1.2f);
    _vec3 vNotch(0.f, 0.3f, -0.5f);

    m_vecPlaneMesh.push_back({ vTip, 0xFF4FA0FF });
    m_vecPlaneMesh.push_back({ vLeft, 0xFF4FA0FF });
    m_vecPlaneMesh.push_back({ vNotch, 0xFF4FA0FF });

    m_vecPlaneMesh.push_back({ vTip, 0xFF1F5FC0 });
    m_vecPlaneMesh.push_back({ vNotch, 0xFF1F5FC0 });
    m_vecPlaneMesh.push_back({ vRight, 0xFF1F5FC0 });
}

void CMiniGame::Add_Quad(vector<MGVTX>& vecMesh, const _vec3& vCenter, _float fHalfX, _float fHalfY, DWORD dwColor)
{
    MGVTX tLT{ _vec3(vCenter.x - fHalfX, vCenter.y + fHalfY, vCenter.z), dwColor };
    MGVTX tRT{ _vec3(vCenter.x + fHalfX, vCenter.y + fHalfY, vCenter.z), dwColor };
    MGVTX tRB{ _vec3(vCenter.x + fHalfX, vCenter.y - fHalfY, vCenter.z), dwColor };
    MGVTX tLB{ _vec3(vCenter.x - fHalfX, vCenter.y - fHalfY, vCenter.z), dwColor };

    vecMesh.push_back(tLT);
    vecMesh.push_back(tRT);
    vecMesh.push_back(tRB);
    vecMesh.push_back(tLT);
    vecMesh.push_back(tRB);
    vecMesh.push_back(tLB);
}

void CMiniGame::Spawn_Gates()
{
    _int iFirst = rand() % MG_GATE_TYPE_COUNT;
    _int iSecond = (iFirst + 1 + rand() % (MG_GATE_TYPE_COUNT - 1)) % MG_GATE_TYPE_COUNT;

    ++m_iGatePairId;

    MGGATE tLeft;
    tLeft.vPos = { -(MG_GATE_HALF_X + 0.5f), MG_GATE_CENTER_Y, 115.f };
    tLeft.iType = iFirst;
    tLeft.iPairId = m_iGatePairId;
    m_vecGates.push_back(tLeft);

    MGGATE tRight = tLeft;
    tRight.vPos.x = MG_GATE_HALF_X + 0.5f;
    tRight.iType = iSecond;
    m_vecGates.push_back(tRight);
}

void CMiniGame::Apply_Gate(_int iType)
{
    switch (iType)
    {
    case 0:
        m_iDamage = min(64, m_iDamage * 2);
        break;
    case 1:
        m_iBulletCount = min(7, m_iBulletCount + 1);
        break;
    case 2:
        m_fFireInterval = max(0.04f, m_fFireInterval * 0.8f);
        break;
    }
}

void CMiniGame::Update_Gates(_float fTimeDelta)
{
    if (!m_bGameOver)
    {
        m_fGateTimer -= fTimeDelta;
        if (m_fGateTimer <= 0.f)
        {
            Spawn_Gates();
            m_fGateTimer = MG_Random(8.f, 11.f);
        }
    }

    _int iPassedPair = -1;

    for (auto& tGate : m_vecGates)
    {
        _float fPrevZ = tGate.vPos.z;
        tGate.vPos.z -= CMiniGameBG::SCROLL_SPEED * fTimeDelta;

        if (m_bGameOver || iPassedPair != -1)
            continue;

        if (fPrevZ > m_vPlayerPos.z && tGate.vPos.z <= m_vPlayerPos.z &&
            fabsf(m_vPlayerPos.x - tGate.vPos.x) <= MG_GATE_HALF_X &&
            fabsf(m_vPlayerPos.y - tGate.vPos.y) <= MG_GATE_HALF_Y)
        {
            Apply_Gate(tGate.iType);
            Spawn_Explosion(m_vPlayerPos + _vec3(0.f, 0.f, 1.f), 1.f);
            iPassedPair = tGate.iPairId;
        }
    }

    for (auto iter = m_vecGates.begin(); iter != m_vecGates.end(); )
    {
        if (iter->iPairId == iPassedPair || iter->vPos.z < -15.f)
            iter = m_vecGates.erase(iter);
        else
            ++iter;
    }
}

void CMiniGame::Render_Gates()
{
    if (m_vecGates.empty())
        return;

    vector<MGVTX> vecMesh;
    for (auto& tGate : m_vecGates)
    {
        const _float fBorder = 0.15f;
        Add_Quad(vecMesh, tGate.vPos, MG_GATE_HALF_X, MG_GATE_HALF_Y, MG_GATE_COLOR[tGate.iType]);
        Add_Quad(vecMesh, tGate.vPos + _vec3(0.f, MG_GATE_HALF_Y, 0.f), MG_GATE_HALF_X + fBorder, fBorder, 0xFFFFFFFF);
        Add_Quad(vecMesh, tGate.vPos - _vec3(0.f, MG_GATE_HALF_Y, 0.f), MG_GATE_HALF_X + fBorder, fBorder, 0xFFFFFFFF);
        Add_Quad(vecMesh, tGate.vPos + _vec3(MG_GATE_HALF_X, 0.f, 0.f), fBorder, MG_GATE_HALF_Y + fBorder, 0xFFFFFFFF);
        Add_Quad(vecMesh, tGate.vPos - _vec3(MG_GATE_HALF_X, 0.f, 0.f), fBorder, MG_GATE_HALF_Y + fBorder, 0xFFFFFFFF);
    }

    _matrix matIdentity;
    D3DXMatrixIdentity(&matIdentity);

    m_pGraphicDev->SetTexture(0, nullptr);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matIdentity);
    m_pGraphicDev->SetFVF(MG_FVF);
    m_pGraphicDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, (UINT)(vecMesh.size() / 3), vecMesh.data(), sizeof(MGVTX));

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
}

void CMiniGame::Render_GateText()
{
    _matrix matViewProj = m_matView * m_matProj;

    for (auto& tGate : m_vecGates)
    {
        if (tGate.vPos.z < 2.f || tGate.vPos.z > 85.f)
            continue;

        _vec3 vScreen;
        D3DXVec3TransformCoord(&vScreen, &tGate.vPos, &matViewProj);

        const _tchar* pText = MG_GATE_TEXT[tGate.iType];
        _float fWidth = 0.f;
        for (const _tchar* pChar = pText; *pChar; ++pChar)
            fWidth += (*pChar < 128) ? 10.f : 20.f;

        _vec2 vPos((vScreen.x + 1.f) * 0.5f * WINCX - fWidth * 0.5f, (1.f - vScreen.y) * 0.5f * WINCY - 10.f);
        _vec2 vShadow = vPos + _vec2(2.f, 2.f);

        CFontMgr::GetInstance()->Render_Font(L"Font_Default", pText, &vShadow, D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
        CFontMgr::GetInstance()->Render_Font(L"Font_Default", pText, &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
    }


    _bool bIsCleared = false;

    if (m_fClearTimer < 3.f)
    {
        bIsCleared = true;
    }
    if (bIsCleared == true)
    {
        _vec2 vClearPos(150.f, 100.f);
        _vec2 vClearShadow = vClearPos + _vec2(2.f, 2.f);


        CFontMgr::GetInstance()->Render_Font(L"Font_Default", L"CLEARED!", &vClearShadow, D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
        CFontMgr::GetInstance()->Render_Font(L"Font_Default", L"CLEARED!", &vClearPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
    }
    else
    {
        wstring strKill = L"KILL : " + to_wstring(m_iKillCount);

        _vec2 vKillPos(20.f, 60.f);
        _vec2 vKillShadow = vKillPos + _vec2(2.f, 2.f);

        CFontMgr::GetInstance()->Render_Font(L"Font_Default", strKill.c_str(), &vKillShadow, D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
        CFontMgr::GetInstance()->Render_Font(L"Font_Default", strKill.c_str(), &vKillPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
    }

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

    for (_int i = CMiniGameBG::TILE_MIN_X; i <= CMiniGameBG::TILE_MAX_X; i++)
    {
        for (_int j = CMiniGameBG::TILE_MIN_Z; j <= CMiniGameBG::TILE_MAX_Z; j++)
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
    Safe_Release(m_pBufferCom);
    Safe_Release(m_pEnemyTex);
    Safe_Release(m_pEnemyBulletTex);
    Safe_Release(m_pBulletTex);
    Safe_Release(m_pExplodeTex);
    Safe_Release(m_pHeartTex);
    Safe_Release(m_pHitTex);

    CScene::Free();
}
