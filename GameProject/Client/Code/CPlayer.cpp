#include "pch.h"
#include "CPlayer.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CDInputMgr.h"
#include "CTerrain.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}


CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    Set_OnTerrain();

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
    Key_Input(fTimeDelta);

    CTerrain* pTerrain = dynamic_cast<CTerrain*>
        (CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"Terrain"));

    if (pTerrain)
    {
        int a = 10;
    }


    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CPlayer::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
   
    m_pTextureCom->Set_Texture(0);
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CPlayer::Add_Component()
{
    CComponent* pComponent = nullptr;

    // RcCol
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_PlayerTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });
    
    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Calculator

    pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Calculator"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });
    return S_OK;
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
    _vec3	vLook;
    m_pTransformCom->Get_Info(INFO_LOOK, &vLook);

    if (GetAsyncKeyState(VK_UP))
    {
        m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), 10.f, fTimeDelta);
    }
    if (GetAsyncKeyState(VK_DOWN))
    {
        m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), -10.f, fTimeDelta);
    }
    
    if (GetAsyncKeyState(VK_LEFT))
    {
        m_pTransformCom->Rotation(ROT_Y, 180.f * fTimeDelta);
    }

    if (GetAsyncKeyState(VK_RIGHT))
    {
        m_pTransformCom->Rotation(ROT_Y, -180.f * fTimeDelta);
    }

    if (CDInputMgr::GetInstance()->Mouse_Press(DIM_LB))
    {
        _vec3   vPickPos = Picking_OnTerrain();

        _vec3   vDir = vPickPos - m_pTransformCom->m_vInfo[INFO_POS];

        m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vDir, &vDir), 10.f, fTimeDelta);
    }
   
}

void CPlayer::Set_OnTerrain()
{
    _vec3   vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);

    CTerrainTex* pTerrainBufferCom = dynamic_cast<CTerrainTex*>
        (CManagement::GetInstance()->Get_Component(ID_STATIC, L"GameLogic_Layer", L"Terrain", L"Com_Buffer"));

    if (nullptr == pTerrainBufferCom)
        return;
    
   _float  fY = m_pCalculatorCom->Compute_HeightOnTerrain(&vPos, pTerrainBufferCom->Get_VtxPos());

   m_pTransformCom->Set_Pos(vPos.x, fY + 1, vPos.z);
}

_vec3 CPlayer::Picking_OnTerrain()
{
    CTerrainTex* pTerrainBufferCom = dynamic_cast<CTerrainTex*>
        (CManagement::GetInstance()->Get_Component(ID_STATIC, L"GameLogic_Layer", L"Terrain", L"Com_Buffer"));

    if (nullptr == pTerrainBufferCom)
        return _vec3(0.f, 0.f, 0.f);

    CTransform* pTerrainTransformCom = dynamic_cast<CTransform*>
        (CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Terrain", L"Com_Transform"));

    if (nullptr == pTerrainTransformCom)
        return _vec3(0.f, 0.f, 0.f);

    return m_pCalculatorCom->Picking_OnTerrain(g_hWnd, pTerrainBufferCom, pTerrainTransformCom);
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CPlayer* pPlayer = new CPlayer(pGraphicDev);

    if (FAILED(pPlayer->Ready_GameObject()))
    {
        Safe_Release(pPlayer);
        MSG_BOX("CPlayer Create Failed");
        return nullptr;
    }

    return pPlayer;
}

void CPlayer::Free()
{
    CGameObject::Free();
}
