#include "pch.h"
#include "CLiminalFrame.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "Client_Enum.h"
#include "CRayCaster.h"
#include "CManagement.h"
#include "CCollisionMgr.h"

CLiminalFrame::CLiminalFrame(LPDIRECT3DDEVICE9 pGraphicDev)
    : CAnamorphicObject(pGraphicDev)
{
}

CLiminalFrame::~CLiminalFrame()
{
}

HRESULT CLiminalFrame::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CAnamorphicObject::Ready_GameObject()))
        return E_FAIL;

    m_pColliderCom->Set_Extents(_vec3{ 0.1f, 0.1f, 0.1f });

    return S_OK;
}

_int CLiminalFrame::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CAnamorphicObject::Update_GameObject(fTimeDelta);

    /* 렌더 그룹, 콜라이더는 부모 클래스에서 */

    return iExit;
}

void CLiminalFrame::LateUpdate_GameObject(_float fTimeDelta)
{
    CAnamorphicObject::LateUpdate_GameObject(fTimeDelta);
}

void CLiminalFrame::Render_GameObject()
{
    /* 아직 지정 안 됐으면 그리지 않음 */
    if (nullptr == m_pBufferCom || nullptr == m_pTextureCom)
        return;

    if (m_b3D)
    {
        if (m_bGrabbed)
        {
            m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pOutlineTransformCom->Get_World());
            m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

            m_pOutlineTextureCom->Set_Texture(100);
            m_pBufferCom->Render_Buffer();

            m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
        }

        m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
        m_pTextureCom->Set_Texture(0);
        m_pBufferCom->Render_Buffer();
    }
    else
    {
        if (m_bCapturePending)
        {
            Render_Capture();
            m_bCapturePending = false;
        }

        Render_Paint();
    }
}

HRESULT CLiminalFrame::SetBuffer(const wstring& wstrTag)
{
    CPlyTex* pBuffer = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(wstrTag.c_str()));

    if (nullptr == pBuffer)
    {
        MSG_BOX("CLiminalFrame::SetBuffer Failed");
        return E_FAIL;
    }

    Remove_Component(ID_STATIC, m_pBufferCom);

    m_pBufferCom = pBuffer;
    m_pBufferCom->Set_Owner(this);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pBuffer });

    return S_OK;
}

HRESULT CLiminalFrame::SetTexture(const wstring& wstrTag)
{
    CTexture* pTexture = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(wstrTag.c_str()));

    if (nullptr == pTexture)
    {
        MSG_BOX("CLiminalFrame::SetTexture Failed");
        return E_FAIL;
    }

    Remove_Component(ID_STATIC, m_pTextureCom);

    m_pTextureCom = pTexture;
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pTexture });

    return S_OK;
}

float CLiminalFrame::SampleTerrainHeight(const _vec3& vRayStart)
{
    if (!m_b3D) return -FLT_MAX;

    CRayCaster* pRayCaster = static_cast<CRayCaster*>(CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"RayCaster"));

    THitInfo tHit{};
    _vec3 vDir{ 0.f, -1.f, 0.f };

    const vector<pair<CVIBuffer*, CTransform*>>& vecInfo = GetRayTestTargetInfo();
    for (auto& [pBuffer, pTransform] : vecInfo)
    {
        pRayCaster->RayTest(tHit, vRayStart, vDir, pBuffer, pTransform->Get_World());
    }

    if (tHit.bHit)
    {
        return tHit.fHitPoint.y;
    }
    else
    {
        return -FLT_MAX;
    }
}

HRESULT CLiminalFrame::Add_Component()
{
    // Collider
    CComponent* pComponent = m_pColliderCom = dynamic_cast<CBoxCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BoxCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });
    
    return S_OK;
}

void CLiminalFrame::Remove_Component(COMPONENTID eID, CComponent* pComponent)
{
    if (nullptr == pComponent)
        return;

    /* 키 타입(포인터/문자열)에 상관없이 값으로 찾아서 제거 */
    auto& mapCom = m_mapComponent[eID];
    for (auto iter = mapCom.begin(); iter != mapCom.end(); ++iter)
    {
        if (iter->second == pComponent)
        {
            Safe_Release(iter->second);
            mapCom.erase(iter);
            return;
        }
    }
}

CLiminalFrame* CLiminalFrame::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CLiminalFrame* pObject = new CLiminalFrame(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CLiminalFrame Create Failed");
        return nullptr;
    }

    return pObject;
}

void CLiminalFrame::Free()
{
    CAnamorphicObject::Free();
}