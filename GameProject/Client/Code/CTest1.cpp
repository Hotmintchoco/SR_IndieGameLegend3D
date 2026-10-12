#include "pch.h"
#include "CTest1.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CTimerMgr.h"
#include "CTerrain.h"
#include "CPlayerCamera.h"
#include "CMiniGame1.h"

CTest1::CTest1(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}


CTest1::~CTest1()
{
}

HRESULT CTest1::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    CGameObject::Ready_GameObject();
    return S_OK;
}

_int CTest1::Update_GameObject(_float fTimeDelta)
{

    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);

    if (m_iIdx == 0 || m_iIdx ==1)
    {
        CTransform* pPlayerTransformCom = static_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC,
            L"GameLogic_Layer", L"Player", L"Com_Transform"));

        _vec3 vPlayerPos; pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

        _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
        _vec3 vDir = vPlayerPos - vPos;
        vDir.y = 0.f;
        D3DXVec3Normalize(&vDir, &vDir);
        vDir *= -1.f;
        _vec3 vAngle;
        vAngle.x = D3DXToDegree(-asinf(vDir.y));
        vAngle.y = D3DXToDegree(atan2f(vDir.x, vDir.z));
        vAngle.z = 0.f;
        m_pTransformCom->Set_Angle(vAngle);
    }
    else if (m_iIdx == 2)
    {
        CTransform* pPlayerTransformCom = static_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC,
            L"GameLogic_Layer", L"Player", L"Com_Transform"));

        _vec3 vPlayerPos; pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

        _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
        _vec3 vDir = vPlayerPos - vPos;

        //_vec3 vDir{-1.f,0.f,0.f};
        vDir.y = 0.f;
        D3DXVec3Normalize(&vDir, &vDir);
        vDir *= -1.f;

        _vec3 vAngle;
        vAngle.x = D3DXToDegree(-asinf(vDir.y));
        vAngle.y = D3DXToDegree(atan2f(vDir.x, vDir.z));
        vAngle.z = 0.f;
        m_pTransformCom->Set_Angle(vAngle);
    }
    else
    {
        CTransform* pPlayerTransformCom = static_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC,
            L"GameLogic_Layer", L"Player", L"Com_Transform"));

        _vec3 vPlayerPos; pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

        _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
        _vec3 vDir = vPlayerPos - vPos;
        vDir.y = 0.f;
        D3DXVec3Normalize(&vDir, &vDir);
        vDir *= -1.f;
        _vec3 vAngle;
        vAngle.x = D3DXToDegree(-asinf(vDir.y));
        vAngle.y = D3DXToDegree(atan2f(vDir.x, vDir.z));
        vAngle.z = 0.f;
        m_pTransformCom->Set_Angle(vAngle);

        _vec3 vDist = vPlayerPos - vPos;

        m_iIdx;
        m_fFrame += fTimeDelta * 10.f;
        if (m_fFrame >= 11.f)
        {
            m_fFrame -= 8.f;
        }

        if (D3DXVec3Length(&vDist) < 0.5f)
        {
            CMiniGame1::Set_Clear();
        }
    }
    return iExit;
}

void CTest1::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CTest1::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    if (m_iIdx == 3)
    {
        m_pTextureCom->Set_Texture((_int)m_fFrame);
    }
    else
    {
        m_pTextureCom->Set_Texture(m_iIdx);
    }
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CTest1::OnCollisionEnter(COLLINFO eCollInfo)
{
}

HRESULT CTest1::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_MG1_Texture"));
    if (nullptr == pComponent) return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // RcCol
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Collider
    pComponent = m_pColliderCom = dynamic_cast<CCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SphereCollider"));
    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    // Calculator
    pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Calculator"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

    return S_OK;
}

CTest1* CTest1::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CTest1* pMonster = new CTest1(pGraphicDev);

    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CTest1 Create Failed");
        return nullptr;
    }

    return pMonster;
}

CTest1* CTest1::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iIdx, _vec3 vScale, _vec3 vPos)
{
    CTest1* pMonster = new CTest1(pGraphicDev);
    pMonster->Set_Idx(iIdx);
    if (FAILED(pMonster->Ready_GameObject()))
    {
        Safe_Release(pMonster);
        MSG_BOX("CTest1 Create Failed");
        return nullptr;
    }
    pMonster->Set_Scale(vScale);
    pMonster->Set_Pos(vPos);

    return pMonster;
}

void CTest1::Free()
{
    CGameObject::Free();
}

void CTest1::Set_Pos(_vec3 vPos)
{
    m_pTransformCom->Set_Pos(vPos);
}

void CTest1::Set_Scale(_vec3 vPos)
{
    m_pTransformCom->Set_Scale(vPos);
}