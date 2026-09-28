#include "pch.h"
#include "CParticle_Sphere.h"
#include "CTransform.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CManagement.h"

CParticle_Sphere::CParticle_Sphere(LPDIRECT3DDEVICE9 pGraphicDev)
    : CParticle(pGraphicDev)
{
}

CParticle_Sphere::~CParticle_Sphere()
{
}

HRESULT CParticle_Sphere::Ready_GameObject()
{
    CParticle::Ready_GameObject();
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_fLifeTime=0.5f;
    m_pTransformCom->Set_Scale(0.f, 0.f, 0.f);

    return S_OK;
}

_int CParticle_Sphere::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CParticle::Update_GameObject(fTimeDelta);
    
    if (m_fElapsedTime > m_fLifeTime)
    {
        Set_Dead(true);
    }

    switch (m_eOption)
    {
    case UP:
		Scale_Up(fTimeDelta);
        break;
    case DOWN:
        Scale_Down(fTimeDelta);
        break;
    case UP_DOWN:   
        if (m_fElapsedTime < m_fLifeTime / 2.f)
        {
            Scale_Up(fTimeDelta);
        }
        else
        {
            Scale_Down(fTimeDelta);
        }
        break;
    }

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    return iExit;
}

void CParticle_Sphere::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CParticle::LateUpdate_GameObject(fTimeDelta);

    _vec3       vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    CGameObject::Compute_ViewZ(&vPos);
}

void CParticle_Sphere::Render_GameObject()
{
    CParticle::Render_GameObject();

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    m_pTextureCom->Set_Texture(m_iOpacity);

    m_pBufferCom->Render_Buffer();
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CParticle_Sphere::Set_Opacity(const _int iOpacity)
{
    m_iOpacity = iOpacity;
}

void CParticle_Sphere::Scale_Up(const _float fTimeDelta)
{
    _float fUp = fTimeDelta * m_fScaleUp_Speed;
    _vec3 vUp = { fUp,fUp,fUp };

    m_pTransformCom->Set_Scale(m_pTransformCom->Get_Scale() + vUp);
}

void CParticle_Sphere::Scale_Down(const _float fTimeDelta)
{
    _float fDown = fTimeDelta * m_fScaleDown_Speed;
    _vec3 vDown = { fDown,fDown,fDown };

    _vec3 vScale = m_pTransformCom->Get_Scale() - vDown;
    if (vScale.x < 0.f) vScale.x = 0.f;
    if (vScale.y < 0.f) vScale.y = 0.f;
    if (vScale.z < 0.f) vScale.z = 0.f;

    m_pTransformCom->Set_Scale(vScale);
}

HRESULT CParticle_Sphere::Add_Component()
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

CParticle_Sphere* CParticle_Sphere::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, EFFECT_SPHERE_COLOR eColor, _int iOpacity)
{
    CParticle_Sphere* pEffect = new CParticle_Sphere(pGraphicDev);
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

CParticle_Sphere* CParticle_Sphere::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, EFFECT_SPHERE_COLOR eColor, _int iOpacity, _float fSpeed, _float fLifeTime)
{
    CParticle_Sphere* pEffect = new CParticle_Sphere(pGraphicDev);
    pEffect->Set_Color(eColor);
    pEffect->Set_Opacity(iOpacity);
    pEffect->Set_ScaleUp_Speed(fSpeed);
    if (FAILED(pEffect->Ready_GameObject()))
    {
        Safe_Release(pEffect);
        MSG_BOX("CEffect_Sphere Create Failed");
        return nullptr;
    }
    pEffect->Set_Pos(vPos);
    pEffect->Set_LifeTime(fLifeTime);

    return pEffect;
}

CParticle_Sphere* CParticle_Sphere::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, EFFECT_SPHERE_COLOR eColor, _int iOpacity, _float fSpeed, _float fLifeTime, _vec3 vScale, EFFECT_SPHERE_OPTION eOption)
{
    CParticle_Sphere* pEffect = new CParticle_Sphere(pGraphicDev);
    pEffect->Set_Color(eColor);
    pEffect->Set_Opacity(iOpacity);
    pEffect->Set_ScaleUp_Speed(fSpeed);
    pEffect->Set_ScaleDown_Speed(fSpeed);
    pEffect->Set_Sphere_Option(eOption);

    if (FAILED(pEffect->Ready_GameObject()))
    {
        Safe_Release(pEffect);
        MSG_BOX("CEffect_Sphere Create Failed");
        return nullptr;
    }
    pEffect->m_pTransformCom->Set_Scale(vScale);
    pEffect->Set_Pos(vPos);
    pEffect->Set_LifeTime(fLifeTime);

    return pEffect;
}

void CParticle_Sphere::Free()
{
    CGameObject::Free();
}
