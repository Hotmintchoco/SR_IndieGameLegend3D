#include "pch.h"
#include "COcto.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CTimerMgr.h"
#include "CTerrain.h"
#include "CSmallExplode.h"
#include "CAbstractFactory.h"
#include "CHeart.h"
#include "CGem.h"
#include "CEnergy.h"
#include "COctoBullet.h"

_bool COcto::sOctoRight = false;
COcto::COcto(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
{
}


COcto::~COcto()
{
}

HRESULT COcto::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    CMonster::Ready_GameObject();

    m_pTransformCom->Set_Scale(0.5f, 0.5f, 0.5f);
    m_pColliderCom->Set_Radius(m_pTransformCom->m_vScale.x);

    m_iMaxHp = 5;
    m_iHp = m_iMaxHp;
    m_bCollision_WithMonster = false;
    sOctoRight = !sOctoRight;
    m_bRightMove = sOctoRight;
    return S_OK;
}

_int COcto::Update_GameObject(_float fTimeDelta)
{
    if (m_bUpdateStart == false)
    {
        m_bUpdateStart = true;
        m_pTransformCom->Get_Info(INFO_POS, &m_vOriginPos);
        LookAtPlayer2();
    }
    if (m_iHp <= 0)
    {
        m_bDelete = true;

        CGameObject* pGameObject = nullptr;
        CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");

        pGameObject = CSmallExplode::Create(m_pGraphicDev, m_pTransformCom->m_vInfo[INFO_POS], m_pTransformCom->m_vScale);
        if (nullptr == pGameObject)
            return E_FAIL;

        if (FAILED(pLayer->Add_GameObject(L"SmallExplode", pGameObject)))
            return E_FAIL;

        //pGameObject = CHeart::Create(m_pGraphicDev, this);
        pGameObject = CGem::Create(m_pGraphicDev, this);
        //pGameObject = CEnergy::Create(m_pGraphicDev, this);
        if (nullptr == pGameObject)
            return E_FAIL;

        if (FAILED(pLayer->Add_GameObject(L"Gem", pGameObject)))
            return E_FAIL;
    }
    _int    iExit = CMonster::Update_GameObject(fTimeDelta);


	Set_OnTerrain();
	m_fFrame += fTimeDelta * 8.f;
	if (m_fFrame > 4.f)
		m_fFrame = 0.f;

    Move_Octo(fTimeDelta);
    Attack_Octo(fTimeDelta);
    LookAtPlayer2();

    return iExit;
}

void COcto::LateUpdate_GameObject(_float fTimeDelta)
{
    CMonster::LateUpdate_GameObject(fTimeDelta);
}

void COcto::Render_GameObject()
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

void COcto::OnCollisionEnter(COLLINFO eCollInfo)
{
    CMonster::OnCollisionEnter(eCollInfo);
}

HRESULT COcto::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_OctoTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void COcto::Move_Octo(_float fTimeDelta)
{
    m_fMoveElapsedTime += fTimeDelta;
    if (m_bMoveFlag == false)
    {
        m_bMoveFlag = true;
        if (m_bMoveOrigin == false)
        {
            _vec3 vRight; m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);
            vRight.y = 0.f;
            D3DXVec3Normalize(&vRight, &vRight);
            if (m_bRightMove == false)
            {
                vRight *= -1;
            }
            m_vMoveDest = m_vOriginPos + vRight * 1.5f;
            m_bRightMove = !m_bRightMove;
        }
        else
        {
            m_vMoveDest = m_vOriginPos;
        }
    }

    _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);

    _vec3 vDist = m_vMoveDest - vPos;
    vDist.y = 0.f;
    if (D3DXVec3Length(&vDist) < 0.125f * 0.5f * 0.5f && m_fMoveElapsedTime > 0.1f)
    {
        //Set_Pos(m_vMoveDest);
        m_bMoveFlag = false;
        m_bMoveOrigin = !m_bMoveOrigin;
        m_fMoveElapsedTime = 0.f;
    }
    else
    {
        _vec3 vDir = m_vMoveDest - vPos;
        vDir.y = 0.f;
        D3DXVec3Normalize(&vDir, &vDir);
        m_pTransformCom->Move_Pos(&vDir, 2.f, fTimeDelta);
    }
}

void COcto::Attack_Octo(_float fTimeDelta)
{
    m_fAttackElapsedTime += fTimeDelta;
    if (m_fAttackElapsedTime > m_fAttackTime)
    {
        m_fAttackElapsedTime = _float(rand() % 128) / 1024.f;

        CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
            ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
        if (nullptr == pPlayerTransformCom) return;
        _vec3   vPlayerPos; pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);
        _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
        vPos.y -= 0.2f;
        _vec3 vDir = vPlayerPos - vPos;
        vDir.y = 0.f;
        D3DXVec3Normalize(&vDir, &vDir);

        CProjectile* pProjectile = COctoBullet::Create(m_pGraphicDev, vPos, vDir);
        CScene* pScene = CManagement::GetInstance()->GetCurrentScene();
        pScene->Add_GameObject(L"Projectile_" + to_wstring(pProjectile->GetProjectileID()), pProjectile);

    }
}

COcto* COcto::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    COcto* pMonster = new COcto(pGraphicDev);

    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("COcto Create Failed");
        return nullptr;
    }

    return pMonster;
}

void COcto::Free()
{
    CMonster::Free();
}