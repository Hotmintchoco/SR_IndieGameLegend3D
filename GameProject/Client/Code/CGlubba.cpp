#include "pch.h"
#include "CGlubba.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CTimerMgr.h"
#include "CTerrain.h"
#include "CAbstractFactory.h"
#include "CPlayerCamera.h"

CGlubba::CGlubba(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
{
}


CGlubba::~CGlubba()
{
}

HRESULT CGlubba::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    CMonster::Ready_GameObject();

    m_pTransformCom->Set_Scale(0.25f, 0.25f, 0.25f);
    m_pColliderCom->Set_Radius(0.5f);
    m_iMaxHp = 2;
    m_iHp = m_iMaxHp;
    m_eMonsterState = JUMP;
    return S_OK;
}

_int CGlubba::Update_GameObject(_float fTimeDelta)
{
    Check_Hp(fTimeDelta);
    Animation_Monster(fTimeDelta);
    _int    iExit = CMonster::Update_GameObject(fTimeDelta);

    switch (m_eMonsterState)
    {
    case IDLE:
        break;
    case JUMP:
        Land(fTimeDelta);
        break;
    case MOVE:
        Set_OnTerrain();
        Chase_Player(fTimeDelta, 0.75f);
        break;
    }

    return iExit;
}

void CGlubba::LateUpdate_GameObject(_float fTimeDelta)
{
    CMonster::LateUpdate_GameObject(fTimeDelta);
}

void CGlubba::Render_GameObject()
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

void CGlubba::OnCollisionEnter(COLLINFO eCollInfo)
{
    CMonster::OnCollisionEnter(eCollInfo);
}

void CGlubba::Check_Hp(_float& fTimedelta)
{
    Check_InRoom();
    if (m_iHp <= 0)
    {
        m_bDelete = true;
        Effect_SmallExplode();
        DropItem();
    }
}

void CGlubba::Animation_Monster(const _float& fTimeDelta)
{
    m_fFrame += fTimeDelta * 5.f;
    if (m_fFrame >= 4.f)
        m_fFrame = 0.f;
}

HRESULT CGlubba::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_glubbaTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}


CGlubba* CGlubba::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CGlubba* pMonster = new CGlubba(pGraphicDev);

    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CGlubba Create Failed");
        return nullptr;
    }
    return pMonster;
}

void CGlubba::Free()
{
    CMonster::Free();
}

void CGlubba::Land(const _float& fTimeDelta)
{
    _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
    _vec3 vScale = m_pTransformCom->Get_Scale();
    m_vLandingDirection.y -= 9.8f * fTimeDelta;

    if (vPos.y < vScale.y)
    {
        vPos.y = vScale.y;
        m_pTransformCom->Set_Pos(vPos);
        m_bLandingState = true;
        m_eMonsterState = MOVE;

    }
    else
    {
        m_pTransformCom->Move_Pos(&m_vLandingDirection, 1.f, fTimeDelta);
    }

    LookAtPlayer();
}