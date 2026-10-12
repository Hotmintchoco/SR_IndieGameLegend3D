#include "pch.h"
#include "CMG0_Herobrine.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CTransform.h"
#include "CMiniGame.h"
#include "CMG0_Player.h"
#include "CMG0_Bullet.h"
#include "CMG0_Enemy_Skull.h"
#include "CMG0_Explosion.h"
#include "CMG0_Laser.h"
#include "CMG0_DancerHead.h"
#include "CMG0_DancerBody.h"
#include "CMG0_DancerLArm.h"
#include "CMG0_DancerRArm.h"
#include "CMG0_DancerLLeg.h"
#include "CMG0_DancerRLeg.h"

CMG0_Herobrine::CMG0_Herobrine(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CMG0_Herobrine::~CMG0_Herobrine()
{
}

HRESULT CMG0_Herobrine::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(Ready_Texture(&m_pTexture, false)))
        return E_FAIL;

    if (FAILED(Ready_Texture(&m_pDeadTexture, true)))
        return E_FAIL;

    if (FAILED(Ready_Part()))
        return E_FAIL;

    Update_Part(0.f);

    return CGameObject::Ready_GameObject();
}

_int CMG0_Herobrine::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CMiniGame* pScene = dynamic_cast<CMiniGame*>(CManagement::GetInstance()->GetCurrentScene());
    if (nullptr == pScene)
        return iExit;

    Update_State(pScene, fTimeDelta);

    if (STATE_WAIT == m_eState || STATE_END == m_eState)
        return iExit;

    Update_Part(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CMG0_Herobrine::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMG0_Herobrine::Render_GameObject()
{
    DWORD dwMinFilter, dwMagFilter;

    m_pGraphicDev->GetSamplerState(0, D3DSAMP_MINFILTER, &dwMinFilter);
    m_pGraphicDev->GetSamplerState(0, D3DSAMP_MAGFILTER, &dwMagFilter);

    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

    m_pGraphicDev->SetTexture(0, Is_Dying() ? m_pDeadTexture : m_pTexture);

    for (CMG0_DancerPart* pPart : m_pPart)
        pPart->Render_GameObject();

    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, dwMagFilter);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, dwMinFilter);
}

void CMG0_Herobrine::Set_Damage(_int iDamage)
{
    if (STATE_IDLE != m_eState)
        return;

    m_iHp = max(0, m_iHp - iDamage);
}

_bool CMG0_Herobrine::Is_Hit(const _vec3& vPos) const
{
    if (STATE_IDLE != m_eState)
        return false;

    const _vec3 vBossPos = MG_BOSS_POS + _vec3(0.f, m_fHeight, 0.f);

    if (fabsf(vPos.x - vBossPos.x) > MG_BOSS_HIT_X || fabsf(vPos.z - vBossPos.z) > MG_BOSS_HIT_Z)
        return false;

    return vPos.y >= vBossPos.y && vPos.y <= vBossPos.y + MG_BOSS_HIT_Y;
}

void CMG0_Herobrine::Update_State(CMiniGame* pScene, _float fTimeDelta)
{
    switch (m_eState)
    {
    case STATE_WAIT:
        if (pScene->Get_SceneTime() >= MG_BOSS_ENTER_START)
        {
            m_eState = STATE_ENTER;
            m_fStateTime = 0.f;
        }
        break;
    case STATE_ENTER:
    {
        m_fStateTime += fTimeDelta;

        _float fRate = min(1.f, m_fStateTime / MG_BOSS_ENTER_TIME);
        _float fEase = 1.f - powf(1.f - fRate, 3.f);

        m_fHeight = MG_BOSS_ENTER_HEIGHT * (1.f - fEase);

        if (fRate >= 1.f)
        {
            m_eState = STATE_IDLE;
            m_fStateTime = 0.f;
        }
        break;
    }
    case STATE_IDLE:
        m_fStateTime += fTimeDelta;
        m_fHeight = MG_BOSS_FLOAT_HEIGHT * (1.f - cosf(m_fStateTime * MG_BOSS_FLOAT_SPEED));

        if (m_iHp <= 0)
        {
            m_eState = STATE_DEAD;
            m_fStateTime = 0.f;

            pScene->Clear_Enemy();
            break;
        }

        if (!pScene->Is_TimeOver() && !pScene->Get_Player()->Is_GameOver())
            Update_Pattern(pScene, fTimeDelta);
        break;
    case STATE_DEAD:
    case STATE_EXPLODE:
        Update_Dead(pScene, fTimeDelta);
        break;
    default:
        break;
    }
}

void CMG0_Herobrine::Update_Part(_float fTimeDelta)
{
    m_pTransformCom->Set_Pos(MG_BOSS_POS + _vec3(0.f, m_fHeight, 0.f) + m_vShake);

    const _matrix& matRoot = *m_pTransformCom->Get_World();
    const MINIGAME0::MGDANCEBEAT tBeat{};

    for (CMG0_DancerPart* pPart : m_pPart)
        pPart->Update_Part(tBeat, fTimeDelta, matRoot);
}

void CMG0_Herobrine::Update_Pattern(CMiniGame* pScene, _float fTimeDelta)
{
    if (m_fPatternRest > 0.f)
    {
        m_fPatternRest -= fTimeDelta;
        return;
    }

    m_fPatternTime += fTimeDelta;

    _bool bEnd = false;

    switch (m_ePattern[m_iPatternIndex])
    {
    case PATTERN_SKULL:
        bEnd = Pattern_Skull(pScene);
        break;
    case PATTERN_LASER:
        bEnd = Pattern_Laser(pScene);
        break;
    case PATTERN_FAN:
        bEnd = Pattern_Fan(pScene);
        break;
    case PATTERN_SIDE_LASER:
        bEnd = Pattern_SideLaser(pScene);
        break;
    case PATTERN_RING:
        bEnd = Pattern_Ring(pScene);
        break;
    default:
        bEnd = true;
        break;
    }

    if (bEnd)
        Next_Pattern();
}

void CMG0_Herobrine::Next_Pattern()
{
    m_fPatternTime = 0.f;
    m_fPatternRest = MG_BOSS_PATTERN_REST;
    m_iPatternStep = 0;

    if (++m_iPatternIndex < PATTERN_END)
        return;

    m_iPatternIndex = 0;

    for (_int i = PATTERN_END; i > 1; --i)
        swap(m_ePattern[i - 1], m_ePattern[rand() % i]);
}

_bool CMG0_Herobrine::Is_Step(_float fGap, _int iCount)
{
    if (m_iPatternStep >= iCount || m_fPatternTime < m_iPatternStep * fGap)
        return false;

    ++m_iPatternStep;

    return true;
}

_bool CMG0_Herobrine::Pattern_Skull(CMiniGame* pScene)
{
    if (Is_Step(0.6f, 2))
    {
        const _vec3 vCore = Get_CorePos();

        for (_int i = 0; i < 2; ++i)
        {
            _float fSide = (0 == i) ? -1.f : 1.f;
            _vec3 vPos = vCore + _vec3(fSide * MINIGAME0::Random(3.f, 7.f), MINIGAME0::Random(-2.f, 3.f), 0.f);

            pScene->Add_GameObject(L"Enemy", CMG0_Enemy_Skull::Create(m_pGraphicDev, vPos, MG_BOSS_SKULL_HP));
        }
    }

    return m_fPatternTime >= 1.2f;
}

_bool CMG0_Herobrine::Pattern_Laser(CMiniGame* pScene)
{
    if (Is_Step(0.f, 1))
    {
        _vec3 vPos = Get_CorePos();
        vPos.y = MG_BOSS_POS.y + MG_BOSS_CORE_Y;

        pScene->Add_GameObject(L"Laser",
            CMG0_Laser::Create(m_pGraphicDev, vPos, MG_BOSS_LASER_RADIUS, MG_BOSS_LASER_WARN_TIME, MG_BOSS_LASER_FIRE_TIME));
    }

    return m_fPatternTime >= MG_BOSS_LASER_WARN_TIME + MG_BOSS_LASER_FIRE_TIME;
}

_bool CMG0_Herobrine::Pattern_Fan(CMiniGame* pScene)
{
    if (Is_Step(0.5f, 3))
    {
        const _vec3 vCore = Get_CorePos();
        const _vec3 vAim = Get_AimDir(pScene);

        for (_int i = -2; i <= 2; ++i)
        {
            _matrix matRot;
            D3DXMatrixRotationY(&matRot, D3DXToRadian(8.f * i));

            _vec3 vDir;
            D3DXVec3TransformNormal(&vDir, &vAim, &matRot);

            pScene->Add_GameObject(L"EnemyBullet",
                CMG0_Bullet::Create(m_pGraphicDev, MINIGAME0::BULLET_ENEMY, vCore, vDir * MG_BOSS_BULLET_SPEED));
        }
    }

    return m_fPatternTime >= 1.4f;
}

_bool CMG0_Herobrine::Pattern_SideLaser(CMiniGame* pScene)
{
    if (Is_Step(0.f, 1))
    {
        _vec3 vPos = Get_CorePos();
        vPos.y = MG_BOSS_POS.y + MG_BOSS_CORE_Y;

        for (_int i = 0; i < 2; ++i)
        {
            vPos.x = (0 == i) ? -MG_BOSS_SIDE_LASER_X : MG_BOSS_SIDE_LASER_X;

            pScene->Add_GameObject(L"Laser",
                CMG0_Laser::Create(m_pGraphicDev, vPos, MG_BOSS_SIDE_LASER_RADIUS, MG_BOSS_LASER_WARN_TIME, MG_BOSS_LASER_FIRE_TIME));
        }
    }

    return m_fPatternTime >= MG_BOSS_LASER_WARN_TIME + MG_BOSS_LASER_FIRE_TIME;
}

_bool CMG0_Herobrine::Pattern_Ring(CMiniGame* pScene)
{
    if (Is_Step(0.7f, 2))
    {
        const _int iCount = 10;
        const _vec3 vCore = Get_CorePos();
        const _vec3 vAim = Get_AimDir(pScene) * (MG_BOSS_BULLET_SPEED * 0.9f);
        const _float fOffset = D3DX_PI / iCount * m_iPatternStep;

        for (_int i = 0; i < iCount; ++i)
        {
            _float fAngle = D3DX_PI * 2.f / iCount * i + fOffset;
            _vec3 vVelocity = vAim + _vec3(cosf(fAngle), sinf(fAngle), 0.f) * 6.f;

            pScene->Add_GameObject(L"EnemyBullet",
                CMG0_Bullet::Create(m_pGraphicDev, MINIGAME0::BULLET_ENEMY, vCore, vVelocity));
        }
    }

    return m_fPatternTime >= 1.2f;
}

void CMG0_Herobrine::Update_Dead(CMiniGame* pScene, _float fTimeDelta)
{
    m_fStateTime += fTimeDelta;
    m_vShake = _vec3(MINIGAME0::Random(-0.15f, 0.15f), MINIGAME0::Random(-0.15f, 0.15f), 0.f);

    if (STATE_DEAD == m_eState)
    {
        if (m_fStateTime >= MG_BOSS_DEAD_TIME)
        {
            m_eState = STATE_EXPLODE;
            m_fStateTime = 0.f;
            m_fExplodeTime = 0.f;
        }
        return;
    }

    m_fHeight -= 1.5f * fTimeDelta;
    m_fExplodeTime -= fTimeDelta;

    if (m_fExplodeTime <= 0.f)
    {
        m_fExplodeTime = MG_BOSS_EXPLODE_GAP;
        Effect_Explode(pScene, MINIGAME0::Random(2.f, 4.f));
    }

    if (m_fStateTime >= MG_BOSS_EXPLODE_TIME)
    {
        for (_int i = 0; i < 6; ++i)
            Effect_Explode(pScene, MINIGAME0::Random(5.f, 8.f));

        m_eState = STATE_END;
    }
}

void CMG0_Herobrine::Effect_Explode(CMiniGame* pScene, _float fScale)
{
    _vec3 vPos = MG_BOSS_POS + _vec3(MINIGAME0::Random(-MG_BOSS_HIT_X, MG_BOSS_HIT_X),
        m_fHeight + MINIGAME0::Random(0.f, MG_BOSS_HIT_Y), -MG_BOSS_HIT_Z);

    pScene->Add_GameObject(L"Explosion", CMG0_Explosion::Create(m_pGraphicDev, vPos, fScale));
}

_vec3 CMG0_Herobrine::Get_CorePos() const
{
    return MG_BOSS_POS + _vec3(0.f, m_fHeight + MG_BOSS_CORE_Y, -MG_BOSS_HIT_Z);
}

_vec3 CMG0_Herobrine::Get_AimDir(CMiniGame* pScene) const
{
    _vec3 vDir = pScene->Get_Player()->Get_Pos() - Get_CorePos();
    D3DXVec3Normalize(&vDir, &vDir);

    return vDir;
}

HRESULT CMG0_Herobrine::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    m_pTransformCom->Set_Scale(MG_BOSS_SCALE, MG_BOSS_SCALE, MG_BOSS_SCALE);
    m_pTransformCom->Set_Rotation_Raw(_vec3(0.f, 180.f, 0.f));

    return S_OK;
}

HRESULT CMG0_Herobrine::Ready_Texture(LPDIRECT3DTEXTURE9* ppTexture, _bool bDead)
{
    if (FAILED(D3DXCreateTextureFromFileExW(m_pGraphicDev, MG_BOSS_TEX_PATH, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_A8R8G8B8,
        D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, nullptr, nullptr, ppTexture)))
        return E_FAIL;

    D3DSURFACE_DESC tDesc;
    D3DLOCKED_RECT tRect;

    if (FAILED((*ppTexture)->GetLevelDesc(0, &tDesc)) || FAILED((*ppTexture)->LockRect(0, &tRect, nullptr, 0)))
        return E_FAIL;

    if (bDead)
    {
        for (_uint y = 0; y < tDesc.Height; ++y)
        {
            DWORD* pRow = reinterpret_cast<DWORD*>(static_cast<BYTE*>(tRect.pBits) + tRect.Pitch * y);

            for (_uint x = 0; x < tDesc.Width; ++x)
            {
                DWORD dwColor = pRow[x];
                DWORD dwGray = (((dwColor >> 16) & 0xFF) + ((dwColor >> 8) & 0xFF) + (dwColor & 0xFF)) / 6;

                pRow[x] = (dwColor & 0xFF000000) | (dwGray << 16) | (dwGray << 8) | dwGray;
            }
        }
    }

    for (const POINT& tPixel : MINIGAME0::MG_BOSS_EYE_PIXEL)
    {
        if (tPixel.x >= (LONG)tDesc.Width || tPixel.y >= (LONG)tDesc.Height)
            continue;

        DWORD* pRow = reinterpret_cast<DWORD*>(static_cast<BYTE*>(tRect.pBits) + tRect.Pitch * tPixel.y);
        pRow[tPixel.x] = bDead ? 0xFF000000 : 0xFFFFFFFF;
    }

    (*ppTexture)->UnlockRect(0);

    return S_OK;
}

HRESULT CMG0_Herobrine::Ready_Part()
{
    m_pPart[PART_BODY] = CMG0_DancerBody::Create(m_pGraphicDev);
    m_pPart[PART_HEAD] = CMG0_DancerHead::Create(m_pGraphicDev);
    m_pPart[PART_LARM] = CMG0_DancerLArm::Create(m_pGraphicDev);
    m_pPart[PART_RARM] = CMG0_DancerRArm::Create(m_pGraphicDev);
    m_pPart[PART_LLEG] = CMG0_DancerLLeg::Create(m_pGraphicDev);
    m_pPart[PART_RLEG] = CMG0_DancerRLeg::Create(m_pGraphicDev);

    for (CMG0_DancerPart* pPart : m_pPart)
    {
        if (nullptr == pPart)
            return E_FAIL;
    }

    m_pPart[PART_HEAD]->Set_Parent(m_pPart[PART_BODY]);
    m_pPart[PART_LARM]->Set_Parent(m_pPart[PART_BODY]);
    m_pPart[PART_RARM]->Set_Parent(m_pPart[PART_BODY]);

    return S_OK;
}

CMG0_Herobrine* CMG0_Herobrine::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMG0_Herobrine* pBoss = new CMG0_Herobrine(pGraphicDev);

    if (FAILED(pBoss->Ready_GameObject()))
    {
        Safe_Release(pBoss);
        MSG_BOX("CMG0_Herobrine Create Failed");
        return nullptr;
    }

    return pBoss;
}

void CMG0_Herobrine::Free()
{
    for (CMG0_DancerPart*& pPart : m_pPart)
        Safe_Release(pPart);

    Safe_Release(m_pTexture);
    Safe_Release(m_pDeadTexture);

    CGameObject::Free();
}
