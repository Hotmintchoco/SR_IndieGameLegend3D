#include "pch.h"
#include "CAirbubble.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include <ctime>
#include "CPlayerCamera.h"

CAirbubble::CAirbubble(LPDIRECT3DDEVICE9 pGraphicDev)
    : CParticle(pGraphicDev)
{
}

CAirbubble::~CAirbubble()
{
}

HRESULT CAirbubble::Ready_GameObject()
{
    CParticle::Ready_GameObject();
    if (FAILED(Add_Component()))
        return E_FAIL;

    _float f = 0.05f;
    if (m_iAirbubbleType == 0)
        m_fScale = f;
    if (m_iAirbubbleType == 1)
        m_fScale = f * 1.5f;

    _vec3 vScale = { m_fScale ,m_fScale ,m_fScale };
    m_pTransformCom->Set_Scale(vScale);

    return S_OK;
}

_int CAirbubble::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
    if (vPos.y>4.f)
    {
        Set_Dead(true);
    }
    _vec3 vDir = { 0.f,1.f,0.f };
    m_pTransformCom->Move_Pos(&vDir, 1.f, fTimeDelta);
    LookAtPlayer();
    return iExit;
}

void CAirbubble::LateUpdate_GameObject(_float fTimeDelta)
{
    CParticle::LateUpdate_GameObject(fTimeDelta);
}

void CAirbubble::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture((_int)m_fFrame);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CAirbubble::Add_Component()
{

    CComponent* pComponent = nullptr;

    // Texture
    if (m_iAirbubbleType == 0)
        pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Airbubble1_Texture"));
    if (m_iAirbubbleType == 1)
        pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Airbubble2_Texture"));

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

CAirbubble* CAirbubble::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CAirbubble* pAirbubble = new CAirbubble(pGraphicDev);

    if (FAILED(pAirbubble->Ready_GameObject()))
    {
        Safe_Release(pAirbubble);
        MSG_BOX("CAirbubble Create Failed");
        return nullptr;
    }

    return pAirbubble;
}

CAirbubble* CAirbubble::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _int iType)
{
    CAirbubble* pAirbubble = new CAirbubble(pGraphicDev);
    pAirbubble->Set_AirbubbleType(iType);
    if (FAILED(pAirbubble->Ready_GameObject()))
    {
        Safe_Release(pAirbubble);
        MSG_BOX("CAirbubble Create Failed");
        return nullptr;
    }
    pAirbubble->Set_Pos(vPos);

    return pAirbubble;
}

void CAirbubble::Free()
{
    CParticle::Free();
}
