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

    m_pOutlineTransformCom->SetUseLocal(true);
    m_pOutlineTransformCom->Set_Scale(1.03f, 1.03f, 1.03f);
    
    return S_OK;
}

_int CLiminalObject::Update_GameObject(_float fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

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
}

void CLiminalObject::OnCollisionEnter(COLLINFO eCollInfo)
{
    auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;

    // switch (iOtherID)
    // {
    // default:
    //     break;
    // }
}

void CLiminalObject::SetOutlineColor(EColorType eType)
{
    if (m_pOutlineTextureCom)
    {
        auto iter = m_mapComponent[ID_DYNAMIC].find(L"Com_OutlineTexture");
        m_mapComponent[ID_DYNAMIC].erase(iter);
        Safe_Release(m_pOutlineTextureCom);
    }

    const unordered_map<EColorType, wstring> s_mapColor = {
        {EColorType::RED, L"Red"},
        {EColorType::GREEN, L"Green"},
        {EColorType::BLUE, L"Blue"},
        {EColorType::YELLOW, L"Yellow"}
    };

    // Texture
    wstring wstrProtoTag = L"Proto_" + s_mapColor.at(eType) + L"_Texture";

    m_pOutlineTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(wstrProtoTag.c_str()));

    if (nullptr == m_pOutlineTextureCom)
        return;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_OutlineTexture", m_pOutlineTextureCom });
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
    
    // Texture
    pComponent = m_pOutlineTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_White_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_OutlineTexture", pComponent });


    return S_OK;
}

vector<pair<Engine::CVIBuffer*, Engine::CTransform*>> CLiminalObject::GetRayTestTargetInfo()
{
    return vector<pair<Engine::CVIBuffer*, Engine::CTransform*>>{{m_pBufferCom, m_pTransformCom}};
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
