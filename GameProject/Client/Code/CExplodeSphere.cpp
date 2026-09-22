#include "pch.h"
#include "CExplodeSphere.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"

CExplodeSphere::CExplodeSphere(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}


CExplodeSphere::~CExplodeSphere()
{
}

HRESULT CExplodeSphere::Ready_GameObject()
{
    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CExplodeSphere::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    m_fAccTime += fTimeDelta;
    if (m_fAccTime >= m_fLifeTime)
    {
        Set_Dead(true);
    }
    else
    {
        float fRatio = m_fAccTime / m_fLifeTime;
        m_fCurrentScale = m_fStartScale + fRatio * (m_fEndScale - m_fStartScale);
        m_pTransformCom->Set_Scale(_vec3{ m_fCurrentScale , m_fCurrentScale , m_fCurrentScale });

        CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
    }

    return iExit;
}

void CExplodeSphere::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CExplodeSphere::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    
    m_pTextureCom->Set_Texture(0);

    m_pBufferCom->Render_Buffer();
}

void CExplodeSphere::SetPos(const _vec3& vPos)
{
    if (!m_pTransformCom)
    {
        assert(0);
        return;
    }

    m_pTransformCom->Set_Pos(vPos);
}

HRESULT CExplodeSphere::Add_Component()
{
    CComponent* pComponent = nullptr;

    // PlyTex
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Sphere_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ExplodeYellow_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}


CExplodeSphere* CExplodeSphere::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CExplodeSphere* pFrustumExplodeEffect = new CExplodeSphere(pGraphicDev);

    if (FAILED(pFrustumExplodeEffect->Ready_GameObject()))
    {
        Safe_Release(pFrustumExplodeEffect);
        MSG_BOX("CExplodeSphere Create Failed");
        return nullptr;
    }

    return pFrustumExplodeEffect;
}


CExplodeSphere* CExplodeSphere::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos)
{
    CExplodeSphere* pExplodeSphere = new CExplodeSphere(pGraphicDev);

    if (FAILED(pExplodeSphere->Ready_GameObject()))
    {
        Safe_Release(pExplodeSphere);
        MSG_BOX("CExplodeSphere Create Failed");
        return nullptr;
    }

    pExplodeSphere->SetPos(vPos);

    return pExplodeSphere;
}

CExplodeSphere* CExplodeSphere::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, const float fStartScale, const float fEndScale, const float fLifeTime)
{
    CExplodeSphere* pExplodeSphere = new CExplodeSphere(pGraphicDev);

    if (FAILED(pExplodeSphere->Ready_GameObject()))
    {
        Safe_Release(pExplodeSphere);
        MSG_BOX("CExplodeSphere Create Failed");
        return nullptr;
    }

    pExplodeSphere->SetPos(vPos);
    pExplodeSphere->SetScale(fStartScale, fEndScale);
    pExplodeSphere->SetLifeTime(fLifeTime);

    return pExplodeSphere;
}


void CExplodeSphere::Free()
{
    CGameObject::Free();
}
