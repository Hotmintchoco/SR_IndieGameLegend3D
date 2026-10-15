#include "pch.h"
#include "CButton.h"
#include "Client_Enum.h"
#include "CLiminalObject.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CCollisionMgr.h"
#include "CSoundMgr.h"

CButton::CButton(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CButton::~CButton()
{
}

HRESULT CButton::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pCollider->Set_Extents(_vec3{ 0.3f, 0.1f, 0.3f });
    m_pCollider->Set_DiffPos(_vec3{ 0.f, 0.05f, 0.f });

    return S_OK;
}

_int CButton::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CCollisionMgr::GetInstance()->Add_Collider(COLL_ROOMLOGIC, m_pCollider);

    return iExit;
}

void CButton::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
}

void CButton::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransform->Get_World());

    if (m_bPressed)
    {
        m_pDownTexture->Set_Texture(0);
        m_pDownBuffer->Render_Buffer();
    }
    else
    {
        m_pUpTexture->Set_Texture(0);
        m_pUpBuffer->Render_Buffer();
    }
}

void CButton::OnCollisionEnter(COLLINFO eCollInfo)
{
    auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;

    if (pOtherCol == m_pCollider) return;

    bool bValid = false;

    switch (iOtherID)
    {
    case COLLISIONID::COLL_ROOMLOGIC:
        break;
    case COLLISIONID::COLL_PLAYER:
        bValid = true;
        break;
    default:
        break;
    }

    if (!bValid) return;

    m_bPressed = true;
    CSoundMgr::GetInstance()->PlaySFX(L"sfxType.wav");
    m_OnButtonPressed.Broadcast();
}

vector<pair<Engine::CVIBuffer*, Engine::CTransform*>> CButton::GetRayTestTargetInfo()
{
    if (m_bPressed)
    {
        return vector<pair<Engine::CVIBuffer*, Engine::CTransform*>>{{m_pDownBuffer, m_pTransform}};
    }
    else
    {
        return vector<pair<Engine::CVIBuffer*, Engine::CTransform*>>{{m_pUpBuffer, m_pTransform}};
    }
}

CVIBuffer* CButton::GetBuffer()
{
    if (m_bPressed)
    {
        return m_pDownBuffer;
    }
    else
    {
        return m_pUpBuffer;
    }
}

CTransform* CButton::GetTransform()
{
    return m_pTransform;
}

HRESULT CButton::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransform = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Vertex (Up)
    pComponent = m_pUpBuffer = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Button_Up_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_UpBuffer", pComponent });

    // Vertex (Down)
    pComponent = m_pDownBuffer = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Button_Down_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_DownBuffer", pComponent });

    // Texture (Up)
    pComponent = m_pUpTexture = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Button_Up_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_UpTexture", pComponent });

    // Texture (Down)
    pComponent = m_pDownTexture = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Button_Down_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_DownTexture", pComponent });

    // Collider
    pComponent = m_pCollider = dynamic_cast<CBoxCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BoxCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_pCollider->Set_Owner(this);

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    return S_OK;
}

CButton* CButton::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CButton* pButton = new CButton(pGraphicDev);

    if (FAILED(pButton->Ready_GameObject()))
    {
        Safe_Release(pButton);
        MSG_BOX("CButton Create Failed");
        return nullptr;
    }

    return pButton;
}

void CButton::Free()
{
    CGameObject::Free();
}