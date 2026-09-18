#include "pch.h"
#include "CFireball.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CTimerMgr.h"
#include "CTerrain.h"
#include "CSmallExplode.h"
#include "CAbstractFactory.h"
#include "CHeart.h"
#include "CGem.h"
#include "CEnergy.h"

CFireball::CFireball(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev), m_fLandingTime(0.f), m_iLandingCount(0), m_fLandingVelocity(0.f)
{
}


CFireball::~CFireball()
{
}

HRESULT CFireball::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    CMonster::Ready_GameObject();

    m_pTransformCom->Set_Scale(0.25f, 0.25f, 0.25f);
    m_pColliderCom->Set_Radius(0.f);
    m_iHp = 100;
    return S_OK;
}

_int CFireball::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CMonster::Update_GameObject(fTimeDelta);

	m_fFrame += fTimeDelta * 10.f;
	if (m_fFrame > 4.f)
		m_fFrame = 0.f;


	Throw(fTimeDelta);

    if (m_iLandingCount == 5)
    {
        Set_Dead(true);
        //지형파괴로직
    }

    return iExit;
}

void CFireball::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CMonster::LateUpdate_GameObject(fTimeDelta);

	CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
		->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

	if (nullptr == pPlayerTransformCom)
		return;

	_vec3   vPlayerPos;
	pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

	_vec3   vPlayerLook;
	pPlayerTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);

	m_pTransformCom->LookAt_Player(&vPlayerPos, &vPlayerLook);
}

void CFireball::Render_GameObject()
{
    CMonster::Render_GameObject();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture((_uint)m_fFrame);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CFireball::OnCollisionEnter(CGameObject* pOther)
{
    //CMonster::OnCollisionEnter(pOther);
}

HRESULT CFireball::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_fireballTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}


CFireball* CFireball::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CFireball* pMonster = new CFireball(pGraphicDev);

    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CFireball Create Failed");
        return nullptr;
    }

    return pMonster;
}

void CFireball::Throw(const _float& fTimeDelta)
{
    m_fLandingTime += fTimeDelta;

    _vec3 vPos, vVelocity;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_fLandingVelocity -= 9.8f * fTimeDelta;

    vVelocity.x = m_vVelocity.x;
    vVelocity.y = m_vVelocity.y + m_fLandingVelocity;
    vVelocity.z = m_vVelocity.z;

    m_vVelocity.y -= 9.8f * fTimeDelta;

    if (m_pTransformCom->m_vInfo[INFO_POS].y < m_pTransformCom->m_vScale.y)
    {
        ++m_iLandingCount;
        m_pTransformCom->m_vInfo[INFO_POS].y = m_pTransformCom->m_vScale.y;
        m_vVelocity.y = -vVelocity.y / 3.f * 2.f;
        m_fLandingVelocity = 0.f;
        return;
    }
    m_pTransformCom->Move_Pos(&vVelocity, 1.f, fTimeDelta);
}

void CFireball::Free()
{
    CMonster::Free();
}