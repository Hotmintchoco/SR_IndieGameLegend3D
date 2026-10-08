#include "pch.h"
#include "CSprnub2.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CTimerMgr.h"
#include "CTerrain.h"
#include "CAbstractFactory.h"
#include "CPlayerCamera.h"

CSprnub2::CSprnub2(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
{
}


CSprnub2::~CSprnub2()
{
}

HRESULT CSprnub2::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    CMonster::Ready_GameObject();

    m_pTransformCom->Set_Scale(0.35f, 0.35f, 0.35f);
    m_pColliderCom->Set_Radius(m_pTransformCom->m_vScale.x);

    m_iMaxHp = 2;
    m_iHp = m_iMaxHp;
    m_eMonsterState = MOVE;
    return S_OK;
}

_int CSprnub2::Update_GameObject(_float fTimeDelta)
{
    if (m_iHp <= 0)
    {
        m_bDelete = true;
        Effect_SmallExplode();
        DropItem();
    }
    _int    iExit = CMonster::Update_GameObject(fTimeDelta);

    Check_Jump(fTimeDelta);
    switch (m_eMonsterState)
    {
    case IDLE:
        break;
    case JUMP:
        Jump(fTimeDelta);
        LookAtPlayer();
        break;
    case MOVE:
        Set_OnTerrain();
        Move(fTimeDelta);
        break;
    }
    m_fFrame += fTimeDelta * 5.f;
    if (m_fFrame >= 2.f)
        m_fFrame = 0.f;

    return iExit;
}

void CSprnub2::LateUpdate_GameObject(_float fTimeDelta)
{
    CMonster::LateUpdate_GameObject(fTimeDelta);
}

void CSprnub2::Render_GameObject()
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

void CSprnub2::OnCollisionEnter(COLLINFO eCollInfo)
{
    CMonster::OnCollisionEnter(eCollInfo);
}

HRESULT CSprnub2::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_sprnub2Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}


CSprnub2* CSprnub2::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CSprnub2* pMonster = new CSprnub2(pGraphicDev);

    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CSprnub2 Create Failed");
        return nullptr;
    }

    return pMonster;
}

CSprnub2* CSprnub2::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fActiveTime)
{
    CSprnub2* pMonster = new CSprnub2(pGraphicDev);
    pMonster->Set_ActiveTime(fActiveTime);

    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CSprnub3 Create Failed");
        return nullptr;
    }

    return pMonster;
}

void CSprnub2::Free()
{
    CMonster::Free();
}

void CSprnub2::Move(const _float& fTimeDelta)
{
    const TBillBoardInfo& tInfo = m_pBillBoardCamera->GetBillBoardInfo();
    _vec3 vPlayerPos; vPlayerPos = tInfo.vPosition;
    _vec3 vPlayerLook; vPlayerLook = tInfo.vLook;

    if (m_fActiveElapsedTime < m_fActiveTime)
    {
        m_fActiveElapsedTime += fTimeDelta;
        m_pTransformCom->LookAt_Player(&vPlayerPos, &vPlayerLook);
    }
    else
    {
        m_pTransformCom->Chase_Target(&vPlayerPos, &vPlayerLook, 2.f, fTimeDelta);
    }
}

void CSprnub2::Jump(const _float& fTimeDelta)
{
    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_vJumpDirection.y -= 9.8f * fTimeDelta;
    m_fJumpTime += fTimeDelta;
    if (vPos.y < m_pTransformCom->m_vScale.y && m_fJumpTime>0.25f)
    {
        vPos.y = m_pTransformCom->m_vScale.y;
        m_pTransformCom->Set_Pos(vPos);
        m_bLandingState = true;
        m_eMonsterState = MOVE;
    }
    else
    {
        m_pTransformCom->Move_Pos(&m_vJumpDirection, 1.f, fTimeDelta);
    }
}

void CSprnub2::Check_Jump(const _float& fTimeDelta)
{
    if (m_fElapsedTime > m_fJumpInterval)
    {
        _int iRand = rand() % 3;
        m_fElapsedTime = _float(iRand) / 8.f * 9.f;
        iRand = 0;
        if (iRand == 0)
        {
            m_eMonsterState = JUMP;
            m_bLandingState = false;
            m_fJumpTime = 0.f;
            m_fVelocityY = 0.f;

            const TBillBoardInfo& tInfo = m_pBillBoardCamera->GetBillBoardInfo();
            _vec3 vPlayerPos; vPlayerPos = tInfo.vPosition;
            _vec3   vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);

            m_vJumpDirection = vPlayerPos - vPos;
            D3DXVec3Normalize(&m_vJumpDirection, &m_vJumpDirection);
            m_vJumpDirection.y = 3.f;
        }
        else
        {
            m_eMonsterState = MOVE;
        }
    }
}