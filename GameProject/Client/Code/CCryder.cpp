#include "pch.h"
#include "CCryder.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CTimerMgr.h"
#include "CTerrain.h"
#include "CAbstractFactory.h"
#include "CPlayerCamera.h"

CCryder::CCryder(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev), m_bLandingState(false), m_fLandingTime(0.f), m_fVelocityY(0.f), m_vLandingDirection{ 0.f,0.f,0.f }
{
}


CCryder::~CCryder()
{
}

HRESULT CCryder::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    CMonster::Ready_GameObject();

    m_pTransformCom->Set_Scale(0.25f, 0.25f, 0.25f);
    m_pColliderCom->Set_Radius(0.35f);
    m_iMaxHp = 4;
    m_iHp = m_iMaxHp;
    return S_OK;
}

_int CCryder::Update_GameObject(_float fTimeDelta)
{
    if (m_iHp <= 0)
    {
        m_bDelete = true;
        Effect_SmallExplode();
        DropItem();
    }
    _int    iExit = CMonster::Update_GameObject(fTimeDelta);

    if (m_bLandingState == true)
    {
        Set_OnTerrain();
        Chase_Player(fTimeDelta, 3.f);
        m_fFrame += fTimeDelta * 10.f;
        if (m_fFrame >= 4.f)
            m_fFrame = 0.f;
    }
    else
    {
        Land(fTimeDelta);
        LookAtPlayer();
        m_pColliderCom->Set_IsActive(false);
    }

    return iExit;
}

void CCryder::LateUpdate_GameObject(_float fTimeDelta)
{
    CMonster::LateUpdate_GameObject(fTimeDelta);
}

void CCryder::Render_GameObject()
{
    if (m_bHitState == true) CMonster::Enable_HitRenderState();

    CMonster::Render_GameObject();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    if (m_bLandingState == true)
    {
        m_pTextureCom->Set_Texture((_uint)m_fFrame);
    }
    else
    {
        m_pTextureCom->Set_Texture(4);
    }
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    if (m_bHitState == true) CMonster::Disable_HitRenderState();
}

void CCryder::OnCollisionEnter(COLLINFO eCollInfo)
{
    CMonster::OnCollisionEnter(eCollInfo);
}

HRESULT CCryder::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_cryderTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}


CCryder* CCryder::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CCryder* pMonster = new CCryder(pGraphicDev);

    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CCryder Create Failed");
        return nullptr;
    }

    return pMonster;
}

void CCryder::Land(const _float& fTimeDelta)
{
    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_vLandingDirection.y -= 9.8f * fTimeDelta;

    if (m_pTransformCom->m_vInfo[INFO_POS].y < m_pTransformCom->m_vScale.y)
    {
        _float y = m_pTransformCom->m_vScale.y;
        _float x = m_pTransformCom->m_vInfo[INFO_POS].x;
        _float z = m_pTransformCom->m_vInfo[INFO_POS].z;
        m_pTransformCom->Set_Pos(x, y, z);
        m_bLandingState = true;
        return;
    }
    m_pTransformCom->Move_Pos(&m_vLandingDirection, 1.f, fTimeDelta);

}

void CCryder::Free()
{
    CMonster::Free();
}