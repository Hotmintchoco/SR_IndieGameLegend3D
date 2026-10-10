#include "pch.h"
#include "CWalkerboss.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CTimerMgr.h"
#include "CTerrain.h"
#include "CAbstractFactory.h"
#include "CPlayerCamera.h"
#include "CWalkerBullet.h"

CWalkerboss::CWalkerboss(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
{
}


CWalkerboss::~CWalkerboss()
{
}

HRESULT CWalkerboss::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    CMonster::Ready_GameObject();

    m_pTransformCom->Set_Scale(0.5f, 0.5f, 0.5f);
    _vec3 vScale = m_pTransformCom->Get_Scale();
    m_pColliderCom->Set_Radius(vScale.x);

    m_iMaxHp = 4;
    m_iHp = m_iMaxHp;
    m_eMonsterState = MOVE;
    return S_OK;
}

_int CWalkerboss::Update_GameObject(_float fTimeDelta)
{
    Check_Hp(fTimeDelta);
    Animation_Monster(fTimeDelta);
    _int    iExit = CMonster::Update_GameObject(fTimeDelta);

    Check_Attack(fTimeDelta);
    switch (m_eMonsterState)
    {
    case IDLE:
        break;
    case ATTACK:
        Attack(fTimeDelta);
        break;
    case MOVE:
        Move(fTimeDelta);
        break;
    }

    return iExit;
}

void CWalkerboss::LateUpdate_GameObject(_float fTimeDelta)
{
    CMonster::LateUpdate_GameObject(fTimeDelta);
}

void CWalkerboss::Render_GameObject()
{
    if (m_bHitState == true) CMonster::Enable_HitRenderState();

    CMonster::Render_GameObject();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture((_uint)m_fFrame);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    if (m_bHitState == true) CMonster::Disable_HitRenderState();
}

void CWalkerboss::OnCollisionEnter(COLLINFO eCollInfo)
{
    CMonster::OnCollisionEnter(eCollInfo);
}

void CWalkerboss::Check_Hp(_float& fTimedelta)
{
    if (m_iHp <= 0)
    {
        m_bDelete = true;
        Effect_SmallExplode();
        DropItem();
    }
}

void CWalkerboss::Animation_Monster(const _float& fTimeDelta)
{
    if (m_eMonsterState == MOVE)
    {
        m_fFrame += fTimeDelta * 10.f;
        if (m_fFrame >= 8.f)
            m_fFrame = 0.f;
    }
    else
    {
        if ((_int)m_fFrame != 0)
        {
            m_fFrame += fTimeDelta * 10.f;
            if (m_fFrame >= 8.f)
                m_fFrame = 0.f;
        }
    }
}

void CWalkerboss::Check_Attack(const _float& fTimeDelta)
{
    if (m_fElapsedTime > m_fAttackInterval)
    {
        _int iRand = rand() % 3;
        m_fElapsedTime = _float(iRand) / 8.f * 9.f;
        m_eMonsterState = ATTACK;
        m_fAttackInterval = 3.f;
    }
}

HRESULT CWalkerboss::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_WalkerbossTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}


CWalkerboss* CWalkerboss::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CWalkerboss* pMonster = new CWalkerboss(pGraphicDev);

    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CWalkerboss Create Failed");
        return nullptr;
    }

    return pMonster;
}

void CWalkerboss::Free()
{
    CMonster::Free();
}

void CWalkerboss::Move(const _float& fTimeDelta)
{
    Set_OnTerrain();
    Chase_Player(fTimeDelta, 0.75f);
}

void CWalkerboss::Attack(const _float& fTimeDelta)
{
    LookAtPlayer2();
    if ((_int)m_fFrame == 0)
    {
        m_fAttackElapsedTime += fTimeDelta;
        if (m_fAttackElapsedTime > 0.75f)
        {
            if (m_bAttackStart == false)
            {
                m_bAttackStart = true;

                const TBillBoardInfo& tInfo = m_pBillBoardCamera->GetBillBoardInfo();
                _vec3   vPlayerPos; vPlayerPos = tInfo.vPosition;
                _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
                vPos.y -= 0.2f;
                _vec3 vDir = vPlayerPos - vPos;
                vDir.y = 0.f;
                D3DXVec3Normalize(&vDir, &vDir);

                _vec3 vRight; m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);
                _vec3 vUp; m_pTransformCom->Get_Info(INFO_UP, &vUp);
                D3DXVec3Normalize(&vRight, &vRight);
                D3DXVec3Normalize(&vUp, &vUp);
                //vPos += _vec3{ 0.4f,0.2f,0.f };
                vPos += vRight * 0.3f + vUp * 0.2f;
                CProjectile* pProjectile = CWalkerBullet::Create(m_pGraphicDev, vPos, vDir);
                CScene* pScene = CManagement::GetInstance()->GetCurrentScene();
                pScene->Add_GameObject(L"Projectile_" + to_wstring(pProjectile->GetProjectileID()), pProjectile);

                //vPos += _vec3{ -0.8f,0.f,0.f };
                vPos += -vRight * 0.6f;
                pProjectile = CWalkerBullet::Create(m_pGraphicDev, vPos, vDir);
                pScene->Add_GameObject(L"Projectile_" + to_wstring(pProjectile->GetProjectileID()), pProjectile);
            }

            if (m_fAttackElapsedTime > 1.25f)
            {
                m_eMonsterState = MOVE;
                m_fAttackElapsedTime = _float(rand() % 16) / 64.f;
                m_bAttackStart = false;
            }
        }
    }
}
