#include "pch.h"
#include "CArrow_Effect.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include <ctime>

CArrow_Effect::CArrow_Effect(LPDIRECT3DDEVICE9 pGraphicDev)
    : CParticle(pGraphicDev)
{
}

CArrow_Effect::~CArrow_Effect()
{
}

HRESULT CArrow_Effect::Ready_GameObject()
{
    CParticle::Ready_GameObject();
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_fLifeTime = 0.5f * 0.5f;

    _float fScale = 0.125f * 0.5f;
    _vec3 vScale = { fScale ,fScale ,fScale };
    m_pTransformCom->Set_Scale(vScale);

    return S_OK;
}

_int CArrow_Effect::Update_GameObject(_float fTimeDelta)
{
    _int    iExit = CParticle::Update_GameObject(fTimeDelta);

    if (m_fElapsedTime / m_fLifeTime > 6.f / 6.f)
    {
        Set_Dead(true);
    }
    else if (m_fElapsedTime / m_fLifeTime > 5.f / 6.f)
    {
        m_iFrame = 2;
    }
    else if (m_fElapsedTime / m_fLifeTime > 4.f / 6.f)
    {
        m_iFrame = 1;
    }
    else if (m_fElapsedTime / m_fLifeTime > 3.f / 6.f)
    {
        m_iFrame = 1;
    }
    else if (m_fElapsedTime / m_fLifeTime > 2.f / 6.f)
    {
        m_iFrame = 0;
    }
    else if (m_fElapsedTime / m_fLifeTime > 1.f / 6.f)
    {
        m_iFrame = 0;
    }



    _vec3 vLook, vUp, vRight;
    D3DXVec3Normalize(&vLook, &m_vDir);
    _vec3 vWorldUp = _vec3{ 0.f, 1.f, 0.f };
    D3DXVec3Cross(&vRight, &vWorldUp, &vLook);
    D3DXVec3Normalize(&vRight, &vRight);
    D3DXVec3Cross(&vUp, &vLook, &vRight);

    _vec3 vScale = m_pTransformCom->Get_Scale();
    vRight = vScale.x * vRight;
    vUp = vScale.y * vUp;
    vLook = vScale.z * vLook;

    _matrix* pWorld = m_pTransformCom->Get_World();
    memcpy(&pWorld->m[0][0], &vRight, sizeof(_vec3));
    memcpy(&pWorld->m[1][0], &vUp, sizeof(_vec3));
    memcpy(&pWorld->m[2][0], &vLook, sizeof(_vec3));

    return iExit;
}

void CArrow_Effect::LateUpdate_GameObject(_float fTimeDelta)
{
    CParticle::LateUpdate_GameObject(fTimeDelta);


}

void CArrow_Effect::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture(m_iFrame);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CArrow_Effect::Add_Component()
{

    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Arrow_Trail_Texture"));

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

CArrow_Effect* CArrow_Effect::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CArrow_Effect* pArrow_Trail = new CArrow_Effect(pGraphicDev);

    if (FAILED(pArrow_Trail->Ready_GameObject()))
    {
        Safe_Release(pArrow_Trail);
        MSG_BOX("CArrow_Trail Create Failed");
        return nullptr;
    }

    return pArrow_Trail;
}

CArrow_Effect* CArrow_Effect::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vDir)
{
    CArrow_Effect* pArrow_Trail = new CArrow_Effect(pGraphicDev);
    pArrow_Trail->Set_Dir(vDir);

    if (FAILED(pArrow_Trail->Ready_GameObject()))
    {
        Safe_Release(pArrow_Trail);
        MSG_BOX("CArrow_Trail Create Failed");
        return nullptr;
    }
    pArrow_Trail->Set_Pos(vPos);

    return pArrow_Trail;
}

void CArrow_Effect::Free()
{
    CParticle::Free();
}
