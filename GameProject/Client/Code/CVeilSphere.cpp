#include "pch.h"
#include "CVeilSphere.h"
#include "CProtoMgr.h"
#include "CAtmosphereVeil.h"
#include "CRenderer.h"

CVeilSphere::CVeilSphere(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CVeilSphere::~CVeilSphere()
{
}

HRESULT CVeilSphere::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    m_pTransformCom->SetUseLocal(true);

	return S_OK;
}

_int CVeilSphere::Update_GameObject(_float fTimeDelta)
{
    if (!Get_IsActive()) return S_OK;

    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CVeilSphere::LateUpdate_GameObject(_float fTimeDelta)
{
    if (!Get_IsActive()) return;

    CGameObject::LateUpdate_GameObject(fTimeDelta);

    m_pTransformCom->WorldMatrixPropagation(*m_pParent->GetTargetTransform()->Get_World());
}

void CVeilSphere::Render_GameObject()
{
    if (!Get_IsActive()) return;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    m_pParent->GetTexture()->Set_Texture(m_iOpacity);

    m_pParent->GetBuffer()->Render_Buffer();
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CVeilSphere::SetRadius(float fRadius)
{
    m_fViewZ = fRadius; /* 알파 소팅값 직접 지정 */
    m_pTransformCom->Set_Scale(_vec3{ fRadius, fRadius, fRadius });
}

HRESULT CVeilSphere::Add_Component()
{
    m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == m_pTransformCom)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", m_pTransformCom });

    return S_OK;
}

CVeilSphere* CVeilSphere::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CVeilSphere* pObject = new CVeilSphere(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CVeilSphere Create Failed");
        return nullptr;
    }

    return pObject;
}

void CVeilSphere::Free()
{
    CGameObject::Free();
}
