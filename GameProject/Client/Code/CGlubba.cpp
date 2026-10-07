#include "pch.h"
#include "CGlubba.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CTimerMgr.h"
#include "CTerrain.h"
#include "CSmallExplode.h"
#include "CAbstractFactory.h"
#include "CHeart.h"
#include "CGem.h"
#include "CEnergy.h"
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
    //m_pColliderCom->Set_Radius(m_pTransformCom->m_vScale.x);
    m_pColliderCom->Set_Radius(0.5f);
    m_iMaxHp = 2;
    m_iHp = m_iMaxHp;
    m_eMonsterState = JUMP;
    return S_OK;
}

_int CGlubba::Update_GameObject(_float fTimeDelta)
{
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

    switch (m_eMonsterState)
    {
    case IDLE:
        break;
    case JUMP:
        Land(fTimeDelta);
        break;
    case MOVE:
        Set_OnTerrain();

        CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
            ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

        if (nullptr == pPlayerTransformCom)
            return E_FAIL;

        const TBillBoardInfo& tInfo = m_pBillBoardCamera->GetBillBoardInfo();

        _vec3   vPlayerPos;
        pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

        _vec3   vPlayerLook;
        vPlayerLook = tInfo.vLook;


		m_pTransformCom->Chase_Target(&vPlayerPos, &vPlayerLook, 0.75f, fTimeDelta);
        
        break;
    }
    m_fFrame += fTimeDelta * 5.f;
    if (m_fFrame > 4.f)
        m_fFrame = 0.f;



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
    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_vLandingDirection.y -= 9.8f * fTimeDelta;

    if (vPos.y < m_pTransformCom->m_vScale.y)
    {
        vPos.y = m_pTransformCom->m_vScale.y;
        m_pTransformCom->Set_Pos(vPos);
        m_bLandingState = true;
        m_eMonsterState = MOVE;

    }
    else
    {
        m_pTransformCom->Move_Pos(&m_vLandingDirection, 1.f, fTimeDelta);
    }

    const TBillBoardInfo& tInfo = m_pBillBoardCamera->GetBillBoardInfo();

    _vec3   vPlayerPos;
    vPlayerPos = tInfo.vPosition;

    _vec3   vPlayerLook;
    vPlayerLook = tInfo.vLook;
    m_pTransformCom->LookAt_Player(&vPlayerPos, &vPlayerLook);
}