#include "pch.h"
#include "CMG0_Sunglass.h"
#include "CProtoMgr.h"
#include "CTransform.h"
#include "CTexture.h"

CMG0_Sunglass::CMG0_Sunglass(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CMG0_Sunglass::~CMG0_Sunglass()
{
}

HRESULT CMG0_Sunglass::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pTransformCom->SetUseLocal(true);
    m_pTransformCom->Set_Scale(MG_SUNGLASS_HALF_X, MG_SUNGLASS_HALF_Y, 1.f);
    m_pTransformCom->Set_Rotation_Raw(_vec3(0.f, 180.f, 0.f));
    m_pTransformCom->Set_Pos(MG_SUNGLASS_POS + _vec3(0.f, MG_SUNGLASS_DROP_HEIGHT, 0.f));

    return CGameObject::Ready_GameObject();
}

_int CMG0_Sunglass::Update_GameObject(_float fTimeDelta)
{
    return CGameObject::Update_GameObject(fTimeDelta);
}

void CMG0_Sunglass::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMG0_Sunglass::Render_GameObject()
{
    DWORD dwCull, dwLighting, dwAlphaTest, dwAlphaRef, dwAlphaFunc, dwMinFilter, dwMagFilter;

    m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &dwCull);
    m_pGraphicDev->GetRenderState(D3DRS_LIGHTING, &dwLighting);
    m_pGraphicDev->GetRenderState(D3DRS_ALPHATESTENABLE, &dwAlphaTest);
    m_pGraphicDev->GetRenderState(D3DRS_ALPHAREF, &dwAlphaRef);
    m_pGraphicDev->GetRenderState(D3DRS_ALPHAFUNC, &dwAlphaFunc);
    m_pGraphicDev->GetSamplerState(0, D3DSAMP_MINFILTER, &dwMinFilter);
    m_pGraphicDev->GetSamplerState(0, D3DSAMP_MAGFILTER, &dwMagFilter);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x80);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Set_Texture(0);
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, dwMagFilter);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, dwMinFilter);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, dwAlphaFunc);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, dwAlphaRef);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, dwAlphaTest);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, dwLighting);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, dwCull);
}

void CMG0_Sunglass::Update_Sunglass(_float fTimeDelta, const _matrix& matHead)
{
    m_fDropTime = min(MG_SUNGLASS_DROP_TIME, m_fDropTime + fTimeDelta);

    _float fRate = m_fDropTime / MG_SUNGLASS_DROP_TIME;
    _float fEase = 1.f - (1.f - fRate) * (1.f - fRate);

    m_pTransformCom->Set_Pos(MG_SUNGLASS_POS + _vec3(0.f, MG_SUNGLASS_DROP_HEIGHT * (1.f - fEase), 0.f));
    m_pTransformCom->WorldMatrixPropagation(matHead);
}

HRESULT CMG0_Sunglass::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = CTexture::Create(m_pGraphicDev, TEX_NORMAL, MG_SUNGLASS_PATH);

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    return S_OK;
}

CMG0_Sunglass* CMG0_Sunglass::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMG0_Sunglass* pSunglass = new CMG0_Sunglass(pGraphicDev);

    if (FAILED(pSunglass->Ready_GameObject()))
    {
        Safe_Release(pSunglass);
        MSG_BOX("CMG0_Sunglass Create Failed");
        return nullptr;
    }

    return pSunglass;
}

void CMG0_Sunglass::Free()
{
    CGameObject::Free();
}
