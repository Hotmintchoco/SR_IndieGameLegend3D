#include "pch.h"
#include "CLiminalObject.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CCollisionMgr.h"
#include "Client_Enum.h"

CLiminalObject::CLiminalObject(LPDIRECT3DDEVICE9 pGraphicDev)
    :CGameObject(pGraphicDev)
{
}

CLiminalObject::~CLiminalObject()
{
}

HRESULT CLiminalObject::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
    m_pOutlineTransformCom->SetUseLocal(true);
    m_pOutlineTransformCom->Set_Scale(1.03f, 1.03f, 1.03f);
    m_pBufferCom->Set_Owner(this);

    return S_OK;
}

_int CLiminalObject::Update_GameObject(_float fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
    CCollisionMgr::GetInstance()->Add_Collider(COLL_OBSTACLE, m_pColliderCom);

    return iExit;
}

void CLiminalObject::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    _matrix matWorld = *m_pTransformCom->Get_World();
    m_pOutlineTransformCom->WorldMatrixPropagation(matWorld);
}

void CLiminalObject::Render_GameObject()
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
    m_pTextureCom->Set_Texture(100);
    m_pBufferCom->Render_Buffer();
}

void CLiminalObject::OnCollisionEnter(COLLINFO eCollInfo)
{
    auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;

    switch (iOtherID)
    {
    default:
        break;
    }
}

vector<pair<Engine::CVIBuffer*, Engine::CTransform*>> CLiminalObject::GetRayTestTargetInfo()
{
    return vector<pair<Engine::CVIBuffer*, Engine::CTransform*>>{{m_pBufferCom, m_pTransformCom}};
}

HRESULT CLiminalObject::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Transform
    pComponent = m_pOutlineTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_OutlineTransform", pComponent });

    // Collider
    pComponent = m_pColliderCom = dynamic_cast<CBoxCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BoxCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    // Buffer
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Cube_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Red_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });
    
    // Texture
    pComponent = m_pWhiteTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Yellow_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_WhiteTexture", pComponent });


    return S_OK;
}

CLiminalObject* CLiminalObject::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CLiminalObject* pObject = new CLiminalObject(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CLiminalObject Create Failed");
        return nullptr;
    }

    return pObject;
}

void CLiminalObject::Free()
{
    CGameObject::Free();
}
