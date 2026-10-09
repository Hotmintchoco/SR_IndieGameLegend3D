#include "pch.h"
#include "CSeaweed.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CSphereCollider.h"
#include "CCollisionMgr.h"
#include "CRoomLayer.h"

CSeaweed::CSeaweed(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CSeaweed::~CSeaweed()
{
}

HRESULT CSeaweed::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    _int iRand = rand() % 360;
    _vec3 vAngle = { 0.f,(_float)iRand, 0.f };
    m_pTransformCom->Set_Angle(vAngle);

    _float fScale = 0.5f;
    _vec3 vScale = { fScale,fScale,fScale };
    m_pTransformCom->Set_Scale(vScale);

    return S_OK;
}

_int CSeaweed::Update_GameObject(_float fTimeDelta)
{
    if (m_bStart == false)
    {
        m_bStart = true;
        _vec3 vPos;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        _vec3 vScale = m_pTransformCom->Get_Scale();
        vPos.y = vScale.y;
        m_pTransformCom->Set_Pos(vPos);
    }
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);

    m_fFrame += fTimeDelta * 5.f;
    if (m_fFrame >= 4.f)
        m_fFrame = 0.f;

    return iExit;
}

void CSeaweed::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CSeaweed::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture((_uint)m_fFrame);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CSeaweed::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Seaweed_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // RcTex
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });


    return S_OK;
}

void CSeaweed::OnCollisionEnter(COLLINFO eCollInfo)
{

}

CSeaweed* CSeaweed::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CSeaweed* pTriggerBox = new CSeaweed(pGraphicDev);

    if (FAILED(pTriggerBox->Ready_GameObject()))
    {
        Safe_Release(pTriggerBox);
        MSG_BOX("CSeaweed Create Failed");
        return nullptr;
    }

    return pTriggerBox;
}

void CSeaweed::Free()
{
    CGameObject::Free();
}
