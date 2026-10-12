#include "pch.h"
#include "CMG0_Enemy_Skull.h"
#include "CProtoMgr.h"
#include "CMiniGame.h"
#include "CMG0_Player.h"
#include "CMG0_Bullet.h"

CMG0_Enemy_Skull::CMG0_Enemy_Skull(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMG0_Enemy(pGraphicDev)
{
}

CMG0_Enemy_Skull::~CMG0_Enemy_Skull()
{
}

HRESULT CMG0_Enemy_Skull::Ready_GameObject(const _vec3& vPos, _int iHp)
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    if (FAILED(CMG0_Enemy::Ready_GameObject()))
        return E_FAIL;

    m_vPos = vPos;
    m_fScale = 1.2f;

    m_iMaxHp = iHp;
    m_iHp = m_iMaxHp;

    m_vVelocity = { 0.f, 0.f, -MINIGAME0::Random(16.f, 22.f) };
    m_fPhase = MINIGAME0::Random(0.f, D3DX_PI * 2.f);
    m_fFireCool = MINIGAME0::Random(1.f, 2.5f);

    return S_OK;
}

_int CMG0_Enemy_Skull::Update_GameObject(_float fTimeDelta)
{
    return CMG0_Enemy::Update_GameObject(fTimeDelta);
}

void CMG0_Enemy_Skull::LateUpdate_GameObject(_float fTimeDelta)
{
    CMG0_Enemy::LateUpdate_GameObject(fTimeDelta);
}

void CMG0_Enemy_Skull::Render_GameObject()
{
    CMG0_Enemy::Render_GameObject();
}

void CMG0_Enemy_Skull::Move(_float fTimeDelta)
{
    m_vPos.x += cosf(m_fElapsedTime * 2.f + m_fPhase) * 4.f * fTimeDelta;
    m_vPos.y += sinf(m_fElapsedTime * 1.5f + m_fPhase) * 2.f * fTimeDelta;
    m_vPos += m_vVelocity * fTimeDelta;
}

void CMG0_Enemy_Skull::Attack(_float fTimeDelta, CMG0_Player* pPlayer)
{
    m_fFireCool -= fTimeDelta;

    if (pPlayer->Is_GameOver() || m_fFireCool > 0.f)
        return;
    if (m_vPos.z <= 20.f || m_vPos.z >= 95.f)
        return;

    _vec3 vDir = pPlayer->Get_Pos() - m_vPos;
    D3DXVec3Normalize(&vDir, &vDir);

    Get_Scene()->Add_GameObject(L"EnemyBullet",
        CMG0_Bullet::Create(m_pGraphicDev, MINIGAME0::BULLET_ENEMY, m_vPos, vDir * 24.f));

    m_fFireCool = MINIGAME0::Random(2.f, 3.5f);
}

HRESULT CMG0_Enemy_Skull::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_skull3Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CMG0_Enemy_Skull* CMG0_Enemy_Skull::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, _int iHp)
{
    CMG0_Enemy_Skull* pMonster = new CMG0_Enemy_Skull(pGraphicDev);

    if (FAILED(pMonster->Ready_GameObject(vPos, iHp)))
    {
        Safe_Release(pMonster);
        MSG_BOX("CMG0_Enemy_Skull Create Failed");
        return nullptr;
    }

    return pMonster;
}

void CMG0_Enemy_Skull::Free()
{
    CMG0_Enemy::Free();
}
