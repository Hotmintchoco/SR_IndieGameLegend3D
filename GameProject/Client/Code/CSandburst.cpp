#include "pch.h"
#include "CSandburst.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include <ctime>
#include "CPlayerCamera.h"

CSandburst::CSandburst(LPDIRECT3DDEVICE9 pGraphicDev)
    : CParticle(pGraphicDev)
{
}

CSandburst::~CSandburst()
{
}

HRESULT CSandburst::Ready_GameObject()
{
    CParticle::Ready_GameObject();
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_fScale = 0.9f;
    _vec3 vScale = { m_fScale ,m_fScale ,m_fScale };
    m_pTransformCom->Set_Scale(vScale);

    return S_OK;
}

_int CSandburst::Update_GameObject(_float fTimeDelta)
{
    _int    iExit = CParticle::Update_GameObject(fTimeDelta);

    m_fFrame += 20.f * fTimeDelta;
    if (5.f <= m_fFrame)
    {
        m_fFrame = 4.5f;
        Set_Dead(true);
    }
    LookAtPlayer2_Sandburst();

    return iExit;
}

void CSandburst::LateUpdate_GameObject(_float fTimeDelta)
{
    CParticle::LateUpdate_GameObject(fTimeDelta);
}

void CSandburst::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture((_int)m_fFrame);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CSandburst::Add_Component()
{

    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Sandburst_Texture"));

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

CSandburst* CSandburst::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CSandburst* pSandburst = new CSandburst(pGraphicDev);

    if (FAILED(pSandburst->Ready_GameObject()))
    {
        Safe_Release(pSandburst);
        MSG_BOX("CSandburst Create Failed");
        return nullptr;
    }

    return pSandburst;
}

CSandburst* CSandburst::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CSandburst* pSandburst = new CSandburst(pGraphicDev);
    pSandburst->Set_OriginPos(vPos);
    if (FAILED(pSandburst->Ready_GameObject()))
    {
        Safe_Release(pSandburst);
        MSG_BOX("CSandburst Create Failed");
        return nullptr;
    }
    pSandburst->Set_Pos(vPos);

    return pSandburst;
}

void CSandburst::LookAtPlayer2_Sandburst()
{
    const TBillBoardInfo& tInfo = m_pBillBoardCamera->GetBillBoardInfo();

    _vec3   vPlayerPos; vPlayerPos = tInfo.vPosition;

    _vec3 vPos;
    vPos = m_vOriginPos;
    vPos.y = m_fScale;

    _vec3 vDir = vPlayerPos - vPos;
    vDir.y = 0.f;
    D3DXVec3Normalize(&vDir, &vDir);

    vPos += vDir * 0.25f;
    m_pTransformCom->Set_Pos(vPos);

    _vec3 vAngle;
    vAngle.x = D3DXToDegree(-asinf(vDir.y));
    vAngle.y = D3DXToDegree(atan2f(vDir.x, vDir.z));
    vAngle.z = 0.f;
    m_pTransformCom->Set_Angle(vAngle);
}

void CSandburst::Free()
{
    CParticle::Free();
}
