#include "pch.h"
#include "CMG0_Train14.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CPlyTex.h"
#include "CMiniGame.h"
#include "CMG0_Player.h"
#include "CMG0_Train13.h"
#include "CLayer.h"

CMG0_Train14::CMG0_Train14(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CMG0_Train14::~CMG0_Train14()
{
}

HRESULT CMG0_Train14::Ready_GameObject(_int iLane, _float fSpeed)
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    _float fX = (MINIGAME0::Get_LaneX(iLane) + MINIGAME0::Get_LaneX(iLane + 1)) * 0.5f;

    m_vPos = _vec3(fX, MG_TRAIN_GROUND_Y, MG_SPAWN_Z);
    m_fSpeed = fSpeed;

    m_pTransformCom->Set_Scale(MG_TRAIN14_SCALE, MG_TRAIN14_SCALE, MG_TRAIN14_SCALE);
    m_pTransformCom->Set_Pos(m_vPos);

    return CGameObject::Ready_GameObject();
}

_int CMG0_Train14::Update_GameObject(_float fTimeDelta)
{
    if (Is_Warning())
    {
        m_fWarnTime += fTimeDelta;

        _int iExit = CGameObject::Update_GameObject(fTimeDelta);

        CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

        return iExit;
    }

    m_vPos.z -= m_fSpeed * fTimeDelta;

    if (m_vPos.z < MG_DESPAWN_Z)
    {
        Set_Dead(true);
        return 0;
    }

    m_pTransformCom->Set_Pos(m_vPos);

    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    Crash_Train13();
    Collision_Player();

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CMG0_Train14::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMG0_Train14::Render_GameObject()
{
    if (Is_Warning())
        Render_Warn();
    else
        Render_Train();
}

void CMG0_Train14::Render_Train()
{
    DWORD dwCull, dwLighting, dwFog, dwColorOp, dwColorArg1, dwAlphaOp, dwAlphaArg1;

    m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &dwCull);
    m_pGraphicDev->GetRenderState(D3DRS_LIGHTING, &dwLighting);
    m_pGraphicDev->GetRenderState(D3DRS_FOGENABLE, &dwFog);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwColorOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwColorArg1);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAlphaOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAlphaArg1);

    _float fFogStart = MG_FOG_START;
    _float fFogEnd = MG_FOG_END;
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_FOGENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
    m_pGraphicDev->SetRenderState(D3DRS_FOGCOLOR, MG_FOG_COLOR);
    m_pGraphicDev->SetRenderState(D3DRS_FOGSTART, *reinterpret_cast<DWORD*>(&fFogStart));
    m_pGraphicDev->SetRenderState(D3DRS_FOGEND, *reinterpret_cast<DWORD*>(&fFogEnd));
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Set_Texture(0);
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwAlphaArg1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwAlphaOp);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwColorArg1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, dwColorOp);
    m_pGraphicDev->SetRenderState(D3DRS_FOGENABLE, dwFog);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, dwLighting);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, dwCull);
}

void CMG0_Train14::Render_Warn()
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
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_pGraphicDev->SetTexture(0, nullptr);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

    _float fRate = m_fWarnTime / MG_TRAIN14_WARN_TIME;
    _int iAlpha = (_int)(50.f + 90.f * fRate * (0.5f + 0.5f * sinf(m_fWarnTime * 30.f)));

    Render_Strip(m_vPos.x, MG_LANE_GAP, D3DCOLOR_ARGB(iAlpha, 255, 40, 40));
    Render_Strip(m_vPos.x - MG_LANE_GAP, 0.1f, D3DCOLOR_ARGB(220, 255, 80, 80));
    Render_Strip(m_vPos.x + MG_LANE_GAP, 0.1f, D3DCOLOR_ARGB(220, 255, 80, 80));

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

void CMG0_Train14::Render_Strip(_float fX, _float fHalfX, DWORD dwColor)
{
    const _float fHalfZ = (MG_SPAWN_Z - MG_DESPAWN_Z) * 0.5f;
    const _float fCenterZ = (MG_SPAWN_Z + MG_DESPAWN_Z) * 0.5f;

    _matrix matWorld;
    D3DXMatrixIdentity(&matWorld);

    matWorld.m[0][0] = fHalfX;
    matWorld.m[1][1] = 0.f;
    matWorld.m[1][2] = fHalfZ;
    matWorld.m[2][1] = 1.f;
    matWorld.m[2][2] = 0.f;
    matWorld.m[3][0] = fX;
    matWorld.m[3][1] = MG_TRAIN_GROUND_Y + 0.05f;
    matWorld.m[3][2] = fCenterZ;

    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwColor);
    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
    m_pWarnBufferCom->Render_Buffer();
}

void CMG0_Train14::Crash_Train13()
{
    CMiniGame* pScene = dynamic_cast<CMiniGame*>(CManagement::GetInstance()->GetCurrentScene());
    if (nullptr == pScene)
        return;

    vector<CMG0_Train13*> vecCrash;

    for (auto& Pair : pScene->Get_Layer(L"GameLogic_Layer")->Get_ObjMap())
    {
        CMG0_Train13* pTrain = dynamic_cast<CMG0_Train13*>(Pair.second);
        if (nullptr == pTrain || pTrain->Is_Dead())
            continue;

        const _vec3& vPos = pTrain->Get_Pos();

        if (fabsf(vPos.x - m_vPos.x) < MG_TRAIN14_CRASH_X && fabsf(vPos.z - m_vPos.z) < MG_TRAIN14_CRASH_Z)
            vecCrash.push_back(pTrain);
    }

    for (CMG0_Train13* pTrain : vecCrash)
        pTrain->Explode();
}

void CMG0_Train14::Collision_Player()
{
    CMiniGame* pScene = dynamic_cast<CMiniGame*>(CManagement::GetInstance()->GetCurrentScene());
    if (nullptr == pScene)
        return;

    CMG0_Player* pPlayer = pScene->Get_Player();
    const _vec3& vPlayerPos = pPlayer->Get_Pos();

    if (fabsf(vPlayerPos.x - m_vPos.x) < MG_TRAIN14_HIT_X && fabsf(vPlayerPos.z - m_vPos.z) < MG_TRAIN14_HIT_Z)
        pPlayer->OnHit();
}

HRESULT CMG0_Train14::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Train14_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pWarnBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_WarnBuffer", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Train14_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CMG0_Train14* CMG0_Train14::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iLane, _float fSpeed)
{
    CMG0_Train14* pTrain = new CMG0_Train14(pGraphicDev);

    if (FAILED(pTrain->Ready_GameObject(iLane, fSpeed)))
    {
        Safe_Release(pTrain);
        MSG_BOX("CMG0_Train14 Create Failed");
        return nullptr;
    }

    return pTrain;
}

void CMG0_Train14::Free()
{
    CGameObject::Free();
}
