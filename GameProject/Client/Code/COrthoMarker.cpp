#include "pch.h"
#include "COrthoMarker.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CRcTex.h"
#include "CTexture.h"

COrthoMarker::COrthoMarker(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

COrthoMarker::~COrthoMarker()
{
}

HRESULT COrthoMarker::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    /* RcTex는 -1~1 이라 0.25배 하면 미니 타일 한 칸(0.5) 크기, 바닥에 눕힘 */
    m_pTransform->Set_Scale(0.25f, 0.25f, 0.25f);
    m_pTransform->Set_Rotation_Raw(_vec3{ 90.f, 0.f, 0.f });

    return S_OK;
}

_int COrthoMarker::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);

    return iExit;
}

void COrthoMarker::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void COrthoMarker::Render_GameObject()
{
    if (!m_bVisible) return;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransform->Get_World());
    m_pTexture->Set_Texture(0);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pBuffer->Render_Buffer();
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT COrthoMarker::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Buffer
    pComponent = m_pBuffer = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", pComponent });

    // Transform
    pComponent = m_pTransform = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Texture
    pComponent = m_pTexture = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Marker_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

COrthoMarker* COrthoMarker::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    COrthoMarker* pObject = new COrthoMarker(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("COrthoMarker Create Failed");
        return nullptr;
    }

    return pObject;
}

void COrthoMarker::Free()
{
    CGameObject::Free();
}
