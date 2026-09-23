#include "pch.h"
#include "CSpeyeder.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CTimerMgr.h"
//#include "CDInputMgr.h"
#include "CTerrain.h"
#include "CSmallExplode.h"
#include "CAbstractFactory.h"
#include "CHeart.h"
#include "CGem.h"
#include "CEnergy.h"

CSpeyeder::CSpeyeder(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev), m_bLandingState(false), m_fLandingTime(0.f), m_fVelocityY(0.f), m_vLandingDirection{0.f,0.f,0.f}
{
}


CSpeyeder::~CSpeyeder()
{
}

HRESULT CSpeyeder::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    CMonster::Ready_GameObject();

    m_pTransformCom->Set_Scale(0.25f, 0.25f, 0.25f);
    m_pColliderCom->Set_Radius(m_pTransformCom->m_vScale.x);
    m_iHp = 2;
    return S_OK;
}

_int CSpeyeder::Update_GameObject(const _float& fTimeDelta)
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
    if (m_bLandingState == true)
    {
        Set_OnTerrain();
        m_fFrame += fTimeDelta * 10.f;
        if (m_fFrame > 4.f)
            m_fFrame = 0.f;
    }
    else
    {
        Land(fTimeDelta);
    }
    


    return iExit;
}

void CSpeyeder::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CMonster::LateUpdate_GameObject(fTimeDelta);

    if (m_bLandingState == true)
    {
        CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
            ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

        if (nullptr == pPlayerTransformCom)
            return;

        _vec3   vPlayerPos;
        pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

        _vec3   vPlayerLook;
        pPlayerTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);

        m_pTransformCom->Chase_Target(&vPlayerPos, &vPlayerLook, 2.f, fTimeDelta);
    }
    else
    {
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
}

void CSpeyeder::Render_GameObject()
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

void CSpeyeder::OnCollisionEnter(CGameObject* pOther)
{
    CMonster::OnCollisionEnter(pOther);
}

HRESULT CSpeyeder::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_speyederTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}


CSpeyeder* CSpeyeder::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CSpeyeder* pMonster = new CSpeyeder(pGraphicDev);

    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CSpeyeder Create Failed");
        return nullptr;
    }

    return pMonster;
}

CSpeyeder* CSpeyeder::Create(LPDIRECT3DDEVICE9 pGraphicDev, _bool bSammom)
{
    CSpeyeder* pMonster = new CSpeyeder(pGraphicDev);
    pMonster->Set_Sammon(bSammom);

    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CSpeyeder Create Failed");
        return nullptr;
    }

    return pMonster;
}


void CSpeyeder::Land(const _float& fTimeDelta)
{
    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_vLandingDirection.y -= 9.8f * fTimeDelta;

    if(m_pTransformCom->m_vInfo[INFO_POS].y < m_pTransformCom->m_vScale.y)
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

void CSpeyeder::Free()
{
    CMonster::Free();
}