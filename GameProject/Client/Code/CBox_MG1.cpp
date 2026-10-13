#include "pch.h"
#include "CBox_MG1.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CTimerMgr.h"
#include "CTerrain.h"
#include "CPlayerCamera.h"

CBox_MG1::CBox_MG1(LPDIRECT3DDEVICE9 pGraphicDev)
    : CTile(pGraphicDev)
{
}


CBox_MG1::~CBox_MG1()
{
}

HRESULT CBox_MG1::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    CGameObject::Ready_GameObject();

    m_pTransformCom->Set_Scale(0.5f, 0.5f, 0.5f);
    m_pTransformCom2->Set_Scale(0.5f, 0.5f, 0.5f);
    _vec3 vScale = m_pTransformCom->Get_Scale();
    m_pColliderCom->Set_Radius(vScale.x);

    //_vec3 vPos = { 0.f,0.f,10.5f };
    //m_pTransformCom->Set_Pos(vPos);

    //_vec3 vDir{ 0.f,-1.f,0.f };
    _vec3 vDir = m_vDir;
    D3DXVec3Normalize(&vDir, &vDir);

    _vec3 vAngle;
    vAngle.x = D3DXToDegree(-asinf(vDir.y));
    vAngle.y = D3DXToDegree(atan2f(vDir.x, vDir.z));
    vAngle.z = 0.f;
    m_pTransformCom->Set_Angle(vAngle);
    m_pTransformCom2->Set_Angle(vAngle);

    return S_OK;
}

_int CBox_MG1::Update_GameObject(_float fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);
    //CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
    Compute_ViewZ(&vPos);
    if (m_iTextureIdx == 0 || m_iTextureIdx == 2)
    {
        m_fViewZ += 50;
    }
    return iExit;
}

void CBox_MG1::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CBox_MG1::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    if (m_bConnection == true)
    {
        //m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom2->Get_World());
        m_pTextureCom->Set_Texture(m_iTextureIdx + 1);
    }
    else
    {
        //m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
        m_pTextureCom->Set_Texture(m_iTextureIdx);
    }

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CBox_MG1::OnCollisionEnter(COLLINFO eCollInfo)
{
}

CVIBuffer* CBox_MG1::GetBuffer()
{
    return m_pBufferCom;
}

HRESULT CBox_MG1::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_MG1Tile_Texture"));
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

    // Transform
    pComponent = m_pTransformCom2 = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform2", pComponent });

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

CBox_MG1* CBox_MG1::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CBox_MG1* pGameObject = new CBox_MG1(pGraphicDev);

    if (FAILED(pGameObject->Ready_GameObject()))
    {
        Safe_Release(pGameObject);
        MSG_BOX("CBox_MG1 Create Failed");
        return nullptr;
    }

    return pGameObject;
}

CBox_MG1* CBox_MG1::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos)
{
    CBox_MG1* pGameObject = new CBox_MG1(pGraphicDev);

    if (FAILED(pGameObject->Ready_GameObject()))
    {
        Safe_Release(pGameObject);
        MSG_BOX("CBox_MG1 Create Failed");
        return nullptr;
    }
    pGameObject->Set_Pos(vPos);

    return pGameObject;
}

CBox_MG1* CBox_MG1::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, _int iIndex)
{
    CBox_MG1* pGameObject = new CBox_MG1(pGraphicDev);

    if (FAILED(pGameObject->Ready_GameObject()))
    {
        Safe_Release(pGameObject);
        MSG_BOX("CBox_MG1 Create Failed");
        return nullptr;
    }
    pGameObject->Set_Pos(vPos);
    pGameObject->Set_TextureIdx(iIndex);

    return pGameObject;
}

CBox_MG1* CBox_MG1::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, _int iIndex, _vec3 vDir)
{
    CBox_MG1* pGameObject = new CBox_MG1(pGraphicDev);
    pGameObject->Set_Dir(vDir);

    if (FAILED(pGameObject->Ready_GameObject()))
    {
        Safe_Release(pGameObject);
        MSG_BOX("CBox_MG1 Create Failed");
        return nullptr;
    }
    pGameObject->Set_Pos(vPos);
    pGameObject->Set_TextureIdx(iIndex);

    return pGameObject;
}

void CBox_MG1::Set_Pos(const _vec3& vPos)
{
    m_pTransformCom->Set_Pos(vPos);
    m_pTransformCom2->Set_Pos(vPos);
}

void CBox_MG1::Free()
{
    CGameObject::Free();
}