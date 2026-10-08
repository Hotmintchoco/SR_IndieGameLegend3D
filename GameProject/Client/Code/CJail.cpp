#include "pch.h"
#include "CJail.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CCollisionMgr.h"
#include "Client_Enum.h"

CJail::CJail(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CJail::~CJail()
{
}

HRESULT CJail::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pColliderCom->Set_Owner(this);
    m_pColliderCom->Set_DiffPos(_vec3(0.f, 0.f, -3.f));
    m_pColliderCom->Set_Extents(1.f, 1.f, 1.f);

    m_iDoorTextureIdx = m_iFrameCnt - 1;

    return S_OK;
}

_int CJail::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);
    CCollisionMgr::GetInstance()->Add_Collider(COLL_ROOMLOGIC, m_pColliderCom);

    if (m_bOnAnimation)
    {
        m_fSingleFrameAccTime += fTimeDelta;
        if (m_fSingleFrameAccTime > m_fFrameInterval)
        {
            m_fSingleFrameAccTime -= m_fFrameInterval;
            --m_iDoorTextureIdx;
        }

        if (m_iDoorTextureIdx <= 0)
        {
            m_bOnAnimation = false;
            m_iDoorTextureIdx = 0;
        }
    }

    return iExit;
}

void CJail::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CJail::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    // 바닥/천장 판
    m_pPlateTextureCom->Set_Texture(0);
    m_pPlateBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // 창살
    m_pBarTextureCom->Set_Texture(0);
    m_pBarBufferCom->Render_Buffer();

    // 문
    if (m_iDoorTextureIdx != 0)
    {
        m_pDoorTextureCom->Set_Texture(m_iDoorTextureIdx);
        m_pDoorBufferCom->Render_Buffer();
    }
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CJail::OnCollisionEnter(COLLINFO eCollInfo)
{
    if (nullptr == eCollInfo.pOtherCollider)
        return;

    if (eCollInfo.iOtherID == COLL_PLAYER)
    {
        OpenDoor();
    }
}

HRESULT CJail::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Buffer
    pComponent = m_pDoorBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Jail_Door_Vertex"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_DoorBuffer", pComponent });

    pComponent = m_pPlateBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Jail_Plates_Vertex"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_PlateBuffer", pComponent });

    pComponent = m_pBarBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Jail_Bars_Vertex"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_BarBuffer", pComponent });

    // Texture
    pComponent = m_pDoorTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Door_Texture"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_DoorTexture", pComponent });

    pComponent = m_pPlateTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Gray_Texture"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_PlateTexture", pComponent });

    pComponent = m_pBarTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Jail_Bars_Texture"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_BarTexture", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Collider
    pComponent = m_pColliderCom = dynamic_cast<CBoxCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BoxCollider"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    return S_OK;
}

void CJail::OpenDoor()
{
    if (m_bOnAnimation || m_iDoorTextureIdx == 0)
        return;

    m_bOnAnimation = true;
    m_fSingleFrameAccTime = 0.f;

    m_pColliderCom->Set_IsActive(false);
}

CJail* CJail::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CJail* pJail = new CJail(pGraphicDev);

    if (FAILED(pJail->Ready_GameObject()))
    {
        Safe_Release(pJail);
        MSG_BOX("CJail Create Failed");
        return nullptr;
    }

    return pJail;
}

void CJail::Free()
{
    CGameObject::Free();
}