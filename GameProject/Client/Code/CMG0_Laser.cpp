#include "pch.h"
#include "CMG0_Laser.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CMiniGame.h"
#include "CMG0_Player.h"

CMG0_Laser::CMG0_Laser(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CMG0_Laser::~CMG0_Laser()
{
}

HRESULT CMG0_Laser::Ready_GameObject(const _vec3& vPos, _float fRadius, _float fWarnTime, _float fFireTime)
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_vPos = vPos;
    m_fRadius = fRadius;
    m_fWarnTime = fWarnTime;
    m_fFireTime = fFireTime;

    return CGameObject::Ready_GameObject();
}

_int CMG0_Laser::Update_GameObject(_float fTimeDelta)
{
    m_fTime += fTimeDelta;

    if (m_fTime >= m_fWarnTime + m_fFireTime)
    {
        Set_Dead(true);
        return 0;
    }

    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    if (Is_Fire())
        Collision_Player();

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    return iExit;
}

void CMG0_Laser::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMG0_Laser::Render_GameObject()
{
    DWORD dwCull, dwLighting, dwZWrite, dwAlphaBlend, dwSrcBlend, dwDestBlend, dwAlphaTest, dwTexFactor;
    DWORD dwColorOp, dwColorArg2, dwAlphaOp, dwAlphaArg2;

    m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &dwCull);
    m_pGraphicDev->GetRenderState(D3DRS_LIGHTING, &dwLighting);
    m_pGraphicDev->GetRenderState(D3DRS_ZWRITEENABLE, &dwZWrite);
    m_pGraphicDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlphaBlend);
    m_pGraphicDev->GetRenderState(D3DRS_SRCBLEND, &dwSrcBlend);
    m_pGraphicDev->GetRenderState(D3DRS_DESTBLEND, &dwDestBlend);
    m_pGraphicDev->GetRenderState(D3DRS_ALPHATESTENABLE, &dwAlphaTest);
    m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &dwTexFactor);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG2, &dwColorArg2);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAlphaOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG2, &dwAlphaArg2);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_pGraphicDev->SetTexture(0, nullptr);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

    if (Is_Fire())
    {
        _float fFireTime = m_fTime - m_fWarnTime;
        _float fOpen = min(1.f, fFireTime / 0.12f) * min(1.f, (m_fFireTime - fFireTime) / 0.2f);
        _float fRadius = m_fRadius * fOpen * (1.f + 0.06f * sinf(m_fTime * 45.f));

        Render_Beam(fRadius, D3DCOLOR_ARGB(150, 255, 60, 60));
        Render_Beam(fRadius * 0.6f, D3DCOLOR_ARGB(190, 255, 150, 110));
        Render_Beam(fRadius * 0.25f, D3DCOLOR_ARGB(255, 255, 255, 255));
    }
    else
    {
        _float fRate = m_fTime / m_fWarnTime;
        _int iAlpha = (_int)(40.f + 60.f * fRate * (0.5f + 0.5f * sinf(m_fTime * 30.f)));

        Render_Beam(m_fRadius, D3DCOLOR_ARGB(iAlpha, 255, 40, 40));
        Render_Beam(0.12f, D3DCOLOR_ARGB(220, 255, 80, 80));
    }

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, dwAlphaArg2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwAlphaOp);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, dwColorArg2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, dwColorOp);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwTexFactor);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, dwAlphaTest);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, dwDestBlend);
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, dwSrcBlend);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlphaBlend);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, dwZWrite);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, dwLighting);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, dwCull);
}

void CMG0_Laser::Collision_Player()
{
    CMiniGame* pScene = dynamic_cast<CMiniGame*>(CManagement::GetInstance()->GetCurrentScene());
    if (nullptr == pScene)
        return;

    CMG0_Player* pPlayer = pScene->Get_Player();
    const _vec3& vPlayerPos = pPlayer->Get_Pos();

    _float fX = vPlayerPos.x - m_vPos.x;
    _float fY = vPlayerPos.y - m_vPos.y;
    _float fRadius = m_fRadius + 0.5f;

    if (fX * fX + fY * fY < fRadius * fRadius)
        pPlayer->OnHit();
}

void CMG0_Laser::Render_Beam(_float fRadius, DWORD dwColor)
{
    const _float fHalfZ = (m_vPos.z - MG_DESPAWN_Z) * 0.5f;
    const _float fCenterZ = (m_vPos.z + MG_DESPAWN_Z) * 0.5f;

    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwColor);

    for (_int i = 0; i < 2; ++i)
    {
        _matrix matWorld;
        D3DXMatrixIdentity(&matWorld);

        matWorld.m[0][0] = (0 == i) ? fRadius : 0.f;
        matWorld.m[0][1] = (0 == i) ? 0.f : fRadius;
        matWorld.m[1][0] = 0.f;
        matWorld.m[1][1] = 0.f;
        matWorld.m[1][2] = fHalfZ;
        matWorld.m[2][0] = (0 == i) ? 0.f : 1.f;
        matWorld.m[2][1] = (0 == i) ? 1.f : 0.f;
        matWorld.m[2][2] = 0.f;
        matWorld.m[3][0] = m_vPos.x;
        matWorld.m[3][1] = m_vPos.y;
        matWorld.m[3][2] = fCenterZ;

        m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
        m_pBufferCom->Render_Buffer();
    }
}

HRESULT CMG0_Laser::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    return S_OK;
}

CMG0_Laser* CMG0_Laser::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, _float fRadius, _float fWarnTime, _float fFireTime)
{
    CMG0_Laser* pLaser = new CMG0_Laser(pGraphicDev);

    if (FAILED(pLaser->Ready_GameObject(vPos, fRadius, fWarnTime, fFireTime)))
    {
        Safe_Release(pLaser);
        MSG_BOX("CMG0_Laser Create Failed");
        return nullptr;
    }

    return pLaser;
}

void CMG0_Laser::Free()
{
    CGameObject::Free();
}
