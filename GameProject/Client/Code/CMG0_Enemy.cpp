#include "pch.h"
#include "CMG0_Enemy.h"
#include "CMiniGame.h"
#include "CMG0_Player.h"

CMG0_Enemy::CMG0_Enemy(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMG0_Sprite(pGraphicDev)
{
}

CMG0_Enemy::~CMG0_Enemy()
{
}

HRESULT CMG0_Enemy::Ready_GameObject()
{
    return CMG0_Sprite::Ready_GameObject();
}

_int CMG0_Enemy::Update_GameObject(_float fTimeDelta)
{
    Check_Hp();

    if (m_bDelete == true)
    {
        Effect_Explode(m_vPos, 2.f);
        Set_Dead(true);
        return 0;
    }

    m_fElapsedTime += fTimeDelta;

    Update_HitState(fTimeDelta);
    Move(fTimeDelta);

    CMiniGame* pScene = Get_Scene();
    if (pScene)
    {
        CMG0_Player* pPlayer = pScene->Get_Player();

        Attack(fTimeDelta, pPlayer);
        Collision_Player(pPlayer);
    }

    if (m_vPos.z < MG_DESPAWN_Z)
    {
        Set_Dead(true);
        return 0;
    }

    return CMG0_Sprite::Update_GameObject(fTimeDelta);
}

void CMG0_Enemy::LateUpdate_GameObject(_float fTimeDelta)
{
    CMG0_Sprite::LateUpdate_GameObject(fTimeDelta);
}

void CMG0_Enemy::Render_GameObject()
{
    m_dwTint = m_bHitState ? 0xFFFF6060 : 0xFFFFFFFF;

    CMG0_Sprite::Render_GameObject();
}

void CMG0_Enemy::Check_Hp()
{
    if (m_iHp <= 0)
    {
        m_bDelete = true;
    }
}

void CMG0_Enemy::Set_Damage(_int iDamage)
{
    m_iHp -= iDamage;
    m_bHitState = true;
    m_fHitEffectElapsedTime = 0.f;
}

void CMG0_Enemy::Update_HitState(_float fTimeDelta)
{
    if (m_bHitState == false)
        return;

    m_fHitEffectElapsedTime += fTimeDelta;
    if (m_fHitEffectElapsedTime >= m_fHitEffectTime)
    {
        m_bHitState = false;
        m_fHitEffectElapsedTime = 0.f;
    }
}

void CMG0_Enemy::Collision_Player(CMG0_Player* pPlayer)
{
    if (pPlayer->Is_GameOver())
        return;

    _vec3 vDiff = pPlayer->Get_Pos() - m_vPos;
    _float fRadius = m_fBodyRadius * m_fScale;
    if (D3DXVec3LengthSq(&vDiff) < fRadius * fRadius)
    {
        m_iHp = 0;
        pPlayer->OnHit();
    }
}

void CMG0_Enemy::Free()
{
    CMG0_Sprite::Free();
}
