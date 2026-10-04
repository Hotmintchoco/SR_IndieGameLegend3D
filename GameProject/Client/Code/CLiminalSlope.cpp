#include "pch.h"
#include "CLiminalSlope.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CCollisionMgr.h"
#include "Client_Enum.h"

CLiminalSlope::CLiminalSlope(LPDIRECT3DDEVICE9 pGraphicDev)
    :CLiminalObject(pGraphicDev)
{
}

CLiminalSlope::~CLiminalSlope()
{
}

HRESULT CLiminalSlope::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CLiminalObject::Ready_GameObject()))
        return E_FAIL;

    m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
    m_pBufferCom->Set_Owner(this);

    return S_OK;
}

_int CLiminalSlope::Update_GameObject(_float fTimeDelta)
{
    _int    iExit = CLiminalObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CLiminalSlope::LateUpdate_GameObject(_float fTimeDelta)
{
    CLiminalObject::LateUpdate_GameObject(fTimeDelta);
}

void CLiminalSlope::Render_GameObject()
{
    if (m_bGrabbed)
    {
        m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pOutlineTransformCom->Get_World());
        m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

        m_pWhiteTextureCom->Set_Texture(100);
        m_pBufferCom->Render_Buffer();

        m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    }

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Set_Texture(0);
    m_pBufferCom->Render_Buffer();
}

void CLiminalSlope::OnCollisionEnter(COLLINFO eCollInfo)
{
    auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;

    switch (iOtherID)
    {
    default:
        break;
    }
}

HRESULT CLiminalSlope::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Buffer
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Slope_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Slope_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CLiminalSlope* CLiminalSlope::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CLiminalSlope* pObject = new CLiminalSlope(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CLiminalSlope Create Failed");
        return nullptr;
    }

    return pObject;
}

void CLiminalSlope::Free()
{
    CGameObject::Free();
}
