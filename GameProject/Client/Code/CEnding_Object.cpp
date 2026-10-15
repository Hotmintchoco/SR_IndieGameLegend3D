#include "pch.h"
#include "CEnding_Object.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CSphereCollider.h"
#include "CCollisionMgr.h"
#include "CRoomLayer.h"

CEnding_Object::CEnding_Object(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CEnding_Object::~CEnding_Object()
{
}

HRESULT CEnding_Object::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;


    _float fScale = 0.1f;
    _vec3 vScale = { fScale,fScale,fScale };
    m_pTransformCom->Set_Scale(vScale);

    _vec3 vDir = {0.f,-1.f,0.f};
    D3DXVec3Normalize(&vDir, &vDir);

    _vec3 vAngle;
    vAngle.x = D3DXToDegree(-asinf(vDir.y));
    vAngle.y = D3DXToDegree(atan2f(vDir.x, vDir.z));
    vAngle.z = 0.f;
    m_pTransformCom->Set_Angle(vAngle);


    return S_OK;
}

_int CEnding_Object::Update_GameObject(_float fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);

    m_fFrame += fTimeDelta * 5.f;
    if (m_fFrame >= m_pTextureCom->GetCount())
        m_fFrame = 0.f;

    _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
    _vec3 vDir = m_vDst - vPos;
    if (D3DXVec3Length(&vDir) > 0.001f)
    {
        D3DXVec3Normalize(&vDir, &vDir);

        _float fSpeed = 0.f;
        if (m_iText == 0)
            fSpeed = 1.f;
        else if (m_iText == 1)
            fSpeed = 0.9f;
        else if (m_iText == 2)
            fSpeed = 0.8f;
        else if (m_iText == 3)
            fSpeed = 0.7f;
        else if (m_iText == 4)
            fSpeed = 0.6f;


        m_pTransformCom->Move_Pos(&vDir, fSpeed, fTimeDelta);
    }
    return iExit;
}

void CEnding_Object::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CEnding_Object::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture((_uint)m_fFrame);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CEnding_Object::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Texture
    if(m_iText==0)
		pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_boss1Texture"));
    else if (m_iText == 1)
        pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_OctoTexture"));
    else if (m_iText == 2)
        pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_WalkerbossTexture"));
    else if (m_iText == 3)
        pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_glubbaTexture"));
    else if (m_iText == 4)
        pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_sprnub1Texture"));

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

void CEnding_Object::OnCollisionEnter(COLLINFO eCollInfo)
{

}

CEnding_Object* CEnding_Object::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CEnding_Object* pTriggerBox = new CEnding_Object(pGraphicDev);

    if (FAILED(pTriggerBox->Ready_GameObject()))
    {
        Safe_Release(pTriggerBox);
        MSG_BOX("CEnding_Object Create Failed");
        return nullptr;
    }

    return pTriggerBox;
}

CEnding_Object* CEnding_Object::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iText, _vec3 vPos, _vec3 vDst)
{
    CEnding_Object* pTriggerBox = new CEnding_Object(pGraphicDev);
    pTriggerBox->Set_Dist(vDst);
    pTriggerBox->Set_Text(iText);

    if (FAILED(pTriggerBox->Ready_GameObject()))
    {
        Safe_Release(pTriggerBox);
        MSG_BOX("CEnding_Object Create Failed");
        return nullptr;
    }
    pTriggerBox->Set_Pos(vPos);

    return pTriggerBox;
}

void CEnding_Object::Set_Pos(_vec3 vPos)
{
    m_pTransformCom->Set_Pos(vPos);
}

void CEnding_Object::Free()
{
    CGameObject::Free();
}
