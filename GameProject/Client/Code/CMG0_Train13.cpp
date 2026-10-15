#include "pch.h"
#include "CMG0_Train13.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CTransform.h"
#include "CTexture.h"
#include "CPlyTex.h"
#include "CMiniGame.h"
#include "CMG0_Player.h"
#include "CMG0_Explosion.h"

CMG0_Train13::CMG0_Train13(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CMG0_Train13::~CMG0_Train13()
{
}

HRESULT CMG0_Train13::Ready_GameObject(_int iLane, _float fSpeed)
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_vPos = _vec3(MINIGAME0::Get_LaneX(iLane), MG_TRAIN_GROUND_Y, MG_SPAWN_Z);
    m_fSpeed = fSpeed;

    m_pTransformCom->Set_Scale(MG_TRAIN_SCALE, MG_TRAIN_SCALE, MG_TRAIN_SCALE);
    m_pTransformCom->Set_Pos(m_vPos);

    return CGameObject::Ready_GameObject();
}

_int CMG0_Train13::Update_GameObject(_float fTimeDelta)
{
    m_vPos.z -= m_fSpeed * fTimeDelta;

    if (m_vPos.z < MG_DESPAWN_Z)
    {
        Set_Dead(true);
        return 0;
    }

    m_pTransformCom->Set_Pos(m_vPos);

    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    Collision_Player();

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CMG0_Train13::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMG0_Train13::Render_GameObject()
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

void CMG0_Train13::Explode()
{
    if (Is_Dead())
        return;

    Set_Dead(true);

    CMiniGame* pScene = dynamic_cast<CMiniGame*>(CManagement::GetInstance()->GetCurrentScene());
    if (nullptr == pScene)
        return;

    for (_int i = 0; i < 3; ++i)
    {
        _vec3 vPos = m_vPos + _vec3(MINIGAME0::Random(-1.f, 1.f), 1.5f + MINIGAME0::Random(-1.f, 1.f), MINIGAME0::Random(-1.5f, 1.5f));

        pScene->Add_GameObject(L"Explosion", CMG0_Explosion::Create(m_pGraphicDev, vPos, MINIGAME0::Random(3.f, 5.f)));
    }
}

void CMG0_Train13::Collision_Player()
{
    CMiniGame* pScene = dynamic_cast<CMiniGame*>(CManagement::GetInstance()->GetCurrentScene());
    if (nullptr == pScene)
        return;

    CMG0_Player* pPlayer = pScene->Get_Player();
    const _vec3& vPlayerPos = pPlayer->Get_Pos();

    if (fabsf(vPlayerPos.x - m_vPos.x) < MG_TRAIN_HIT_X && fabsf(vPlayerPos.z - m_vPos.z) < MG_TRAIN_HIT_Z)
        pPlayer->OnHit();
}

HRESULT CMG0_Train13::Add_Component()
{
    CComponent* pComponent = nullptr;

    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Thomas_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Thomas_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CMG0_Train13* CMG0_Train13::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iLane, _float fSpeed)
{
    CMG0_Train13* pTrain = new CMG0_Train13(pGraphicDev);

    if (FAILED(pTrain->Ready_GameObject(iLane, fSpeed)))
    {
        Safe_Release(pTrain);
        MSG_BOX("CMG0_Train13 Create Failed");
        return nullptr;
    }

    return pTrain;
}

void CMG0_Train13::Free()
{
    CGameObject::Free();
}
