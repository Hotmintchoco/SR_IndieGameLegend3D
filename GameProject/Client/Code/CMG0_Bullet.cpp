#include "pch.h"
#include "CMG0_Bullet.h"
#include "CProtoMgr.h"
#include "CLayer.h"
#include "CMiniGame.h"
#include "CMG0_Player.h"
#include "CMG0_Enemy.h"
#include "CMG0_Herobrine.h"

CMG0_Bullet::CMG0_Bullet(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMG0_Sprite(pGraphicDev)
{
}

CMG0_Bullet::~CMG0_Bullet()
{
}

HRESULT CMG0_Bullet::Ready_GameObject(MINIGAME0::BULLETTYPE eType, const _vec3& vPos, const _vec3& vVelocity, _int iDamage)
{
    m_eType = eType;

    if (FAILED(Add_Component()))
        return E_FAIL;
    if (FAILED(CMG0_Sprite::Ready_GameObject()))
        return E_FAIL;

    m_vPos = vPos;
    m_vVelocity = vVelocity;
    m_iDamage = iDamage;

    if (MINIGAME0::BULLET_PLAYER == m_eType)
    {
        m_fScale = 0.3f;
        m_fLifeTime = 2.f;
        m_fFrameSpeed = 15.f;
    }
    else
    {
        m_fScale = 0.5f;
        m_fLifeTime = 6.f;
        m_fFrameSpeed = 12.f;
    }

    return S_OK;
}

_int CMG0_Bullet::Update_GameObject(_float fTimeDelta)
{
    m_vPos += m_vVelocity * fTimeDelta;
    m_fLifeTime -= fTimeDelta;

    m_fFrame += m_fFrameSpeed * fTimeDelta;
    m_iFrame = (_uint)m_fFrame % (_uint)max(1, m_pTextureCom->GetCount());

    _bool bHit = (MINIGAME0::BULLET_PLAYER == m_eType) ? Collision_Enemy() : Collision_Player();

    if (bHit || m_fLifeTime <= 0.f || Is_OutOfRange())
    {
        Set_Dead(true);
        return 0;
    }

    return CMG0_Sprite::Update_GameObject(fTimeDelta);
}

void CMG0_Bullet::LateUpdate_GameObject(_float fTimeDelta)
{
    CMG0_Sprite::LateUpdate_GameObject(fTimeDelta);
}

void CMG0_Bullet::Render_GameObject()
{
    CMG0_Sprite::Render_GameObject();
}

_bool CMG0_Bullet::Is_OutOfRange()
{
    if (MINIGAME0::BULLET_PLAYER == m_eType)
        return m_vPos.z > MG_SPAWN_Z + 5.f;

    return m_vPos.z < MG_DESPAWN_Z;
}

_bool CMG0_Bullet::Collision_Enemy()
{
    CMiniGame* pScene = Get_Scene();
    if (nullptr == pScene)
        return false;

    CLayer* pLayer = pScene->Get_Layer(L"GameLogic_Layer");
    if (nullptr == pLayer)
        return false;

    for (auto& Pair : pLayer->Get_ObjMap())
    {
        CMG0_Enemy* pEnemy = dynamic_cast<CMG0_Enemy*>(Pair.second);
        if (nullptr == pEnemy || pEnemy->Is_Dead() || pEnemy->Get_Hp() <= 0)
            continue;

        _vec3 vDiff = pEnemy->Get_Pos() - m_vPos;
        _float fRadius = pEnemy->Get_HitRadius();
        if (D3DXVec3LengthSq(&vDiff) < fRadius * fRadius)
        {
            pEnemy->Set_Damage(m_iDamage);
            Effect_Explode(m_vPos, 0.5f);
            return true;
        }
    }

    CMG0_Herobrine* pBoss = pScene->Get_Boss();
    if (pBoss->Is_Hit(m_vPos))
    {
        pBoss->Set_Damage(m_iDamage);
        Effect_Explode(m_vPos, 0.5f);
        return true;
    }

    return false;
}

_bool CMG0_Bullet::Collision_Player()
{
    CMiniGame* pScene = Get_Scene();
    if (nullptr == pScene)
        return false;

    CMG0_Player* pPlayer = pScene->Get_Player();
    if (pPlayer->Is_GameOver())
        return false;

    _vec3 vDiff = pPlayer->Get_Pos() - m_vPos;
    if (D3DXVec3LengthSq(&vDiff) < 0.9f * 0.9f)
    {
        Effect_Explode(m_vPos, 0.8f);
        pPlayer->OnHit();
        return true;
    }

    return false;
}

HRESULT CMG0_Bullet::Add_Component()
{
    CComponent* pComponent = nullptr;

    const _tchar* pTextureTag = (MINIGAME0::BULLET_PLAYER == m_eType) ? L"Proto_Bullet_Default_Texture" : L"Proto_fireballTexture";

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(pTextureTag));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CMG0_Bullet* CMG0_Bullet::Create(LPDIRECT3DDEVICE9 pGraphicDev, MINIGAME0::BULLETTYPE eType, const _vec3& vPos, const _vec3& vVelocity, _int iDamage)
{
    CMG0_Bullet* pBullet = new CMG0_Bullet(pGraphicDev);

    if (FAILED(pBullet->Ready_GameObject(eType, vPos, vVelocity, iDamage)))
    {
        Safe_Release(pBullet);
        MSG_BOX("CMG0_Bullet Create Failed");
        return nullptr;
    }

    return pBullet;
}

void CMG0_Bullet::Free()
{
    CMG0_Sprite::Free();
}
