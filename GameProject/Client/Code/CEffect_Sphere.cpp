#include "pch.h"
#include "CEffect_Sphere.h"
#include "CTransform.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CManagement.h"

CEffect_Sphere::CEffect_Sphere(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEffect(pGraphicDev)
{
}

CEffect_Sphere::~CEffect_Sphere()
{
}

HRESULT CEffect_Sphere::Ready_GameObject()
{
    CEffect::Ready_GameObject();
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_fLifeTime=10.f;
    m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);

    return S_OK;
}

_int CEffect_Sphere::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CEffect::Update_GameObject(fTimeDelta);
    
    if (m_fElapsedTime > m_fLifeTime)
    {
        Set_Dead(true);
    }

    Scale_Up(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    return iExit;
}

void CEffect_Sphere::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CEffect::LateUpdate_GameObject(fTimeDelta);

    _vec3       vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    CGameObject::Compute_ViewZ(&vPos);
}

void CEffect_Sphere::Render_GameObject()
{
    CEffect::Render_GameObject();

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    m_pTextureCom->Set_Texture(m_iOpacity);

    m_pBufferCom->Render_Buffer();
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CEffect_Sphere::Set_Scale(const _float fScale)
{
    if (!m_pTransformCom) return;
    m_fScale = fScale;
    m_pTransformCom->Set_Scale(m_fScale, m_fScale, m_fScale);
}

void CEffect_Sphere::Set_Opacity(const _int iOpacity)
{
    m_iOpacity = iOpacity;
}

void CEffect_Sphere::Scale_Up(const _float fTimeDelta)
{
    m_fScale += fTimeDelta * 0.5f * 0.5f;
    Set_Scale(m_fScale);
}

HRESULT CEffect_Sphere::Add_Component()
{
    CComponent* pComponent = nullptr;

    // TerrainTex
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Sphere_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    switch (m_eColor)
    {
    case BLACK:
        pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Black_Texture"));
        break;
	case YELLOW:
        pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Yellow_Texture"));
        break;
    case ORANGE:
        pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Orange_Texture"));
        break;
    case RED:
        pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Red_Texture"));
        break;
    }

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CEffect_Sphere* CEffect_Sphere::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, EFFECT_SPHERE_COLOR eColor, _int iOpacity)
{
    CEffect_Sphere* pEffect = new CEffect_Sphere(pGraphicDev);
    pEffect->Set_Color(eColor);
    pEffect->Set_Opacity(iOpacity);
    if (FAILED(pEffect->Ready_GameObject()))
    {
        Safe_Release(pEffect);
        MSG_BOX("CEffect_Sphere Create Failed");
        return nullptr;
    }
    pEffect->Set_Pos(vPos);

    return pEffect;
}

void CEffect_Sphere::Free()
{
    CGameObject::Free();
}
