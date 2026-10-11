#include "pch.h"
#include "CNpc.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CSphereCollider.h"
#include "CCollisionMgr.h"
#include "CClientCameraMgr.h"
#include "CCamera.h"

CNpc::CNpc(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CNpc::~CNpc()
{
}

HRESULT CNpc::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;



    _float fScale = 0.5f;
    _vec3 vScale = { fScale,fScale,fScale };
    m_pTransformCom->Set_Scale(vScale);

    return S_OK;
}

_int CNpc::Update_GameObject(_float fTimeDelta)
{
    if (m_bStart == false)
    {
        m_bStart = true;
        _vec3 vPos;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        _vec3 vScale = m_pTransformCom->Get_Scale();
        vPos.y += vScale.y;
        m_pTransformCom->Set_Pos(vPos);
    }
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);



    return iExit;
}

void CNpc::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
    BillBoard();
}

void CNpc::BillBoard()
{
    auto* pCamera = CClientCameraMgr::GetInstance()->Get_ActiveCamera();
    if (!pCamera) return;
    _matrix matCamera;
    pCamera->GetWorld(&matCamera);
    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    const _vec3 vDist = vPos - _vec3{ matCamera._41, matCamera._42, matCamera._43 };
    if (vDist.x * vDist.x + vDist.z * vDist.z < 0.000001f) return;
    m_pTransformCom->Set_Rotation_Raw({ 0.f, D3DXToDegree(atan2f(vDist.x, vDist.z)), 0.f });
}

void CNpc::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture(0);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CNpc::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Npc_Texture"));

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

void CNpc::OnCollisionEnter(COLLINFO eCollInfo)
{

}

CNpc* CNpc::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CNpc* pNpc = new CNpc(pGraphicDev);

    if (FAILED(pNpc->Ready_GameObject()))
    {
        Safe_Release(pNpc);
        MSG_BOX("CNpc Create Failed");
        return nullptr;
    }

    return pNpc;
}

void CNpc::Free()
{
    CGameObject::Free();
}
