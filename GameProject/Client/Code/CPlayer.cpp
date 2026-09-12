#include "pch.h"
#include "CPlayer.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CDInputMgr.h"
#include "CTerrain.h"
#include "CSphereCollider.h"
#include "CCollisionMgr.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev), m_iJumpState(JUMP_NOT), m_fJumpTime(0.f), m_bFix(true), m_bCheck(true)
{
}


CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

	__super::Ready_GameObject();

    m_pColliderCom->Set_Radius(1.f);
	m_pTransformCom->Set_Pos(60.f, 1.f, 60.f);

    return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
    _vec3   vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    Compute_ViewZ(&vPos);

    m_vPrevPos = vPos;

    Key_Input(fTimeDelta);

    Set_OnTerrain(fTimeDelta);
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    return iExit;
}

void CPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	// 충돌 처리 여부를 위해 충돌 매니저에 플레이어의 콜라이더를 등록
    CCollisionMgr::GetInstance()->Add_Collider(COLL_PLAYER, m_pColliderCom);
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

void CPlayer::OnCollisionEnter(CGameObject* pOther)
{
	m_pTransformCom->Set_Pos(m_vPrevPos.x, m_vPrevPos.y, m_vPrevPos.z);
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

    // Collider
    pComponent = m_pColliderCom = dynamic_cast<CCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SphereCollider"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    return S_OK;
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
    _vec3	vLook;
    _vec3   vRight;
    m_pTransformCom->Get_Info(INFO_LOOK, &vLook);
    m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);

    _float fSpeed = 5.f;

    if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_LSHIFT))
    {
        fSpeed *= 2;
    }

    if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_W))
    {
        m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fSpeed, fTimeDelta);
    }

    if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_S))
    {
        m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), -fSpeed, fTimeDelta);
    }

    if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_A))
    {
        m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), -fSpeed, fTimeDelta);
    }

    if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_D))
    {
        m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fSpeed, fTimeDelta);
    }

    if ((m_iJumpState == JUMP_NOT) && (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_SPACE)))
    {
        m_iJumpState = JUMP_PARABOLIC;
        m_fJumpTime = 0.f;
    }

    /*
    if (CDInputMgr::GetInstance()->Get_DIMouseState(DIM_LB) & 0x80)
    {
        _vec3   vPickPos = Picking_OnTerrain();

        _vec3   vDir = vPickPos - m_pTransformCom->m_vInfo[INFO_POS];

        m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vDir, &vDir), 10.f, fTimeDelta);
    }
   */

    if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_TAB))
    {
        if (m_bCheck)
            return;

        m_bCheck = true;

        if (m_bFix)
            m_bFix = false;

        else
            m_bFix = true;

    }

    else
    {
        m_bCheck = false;
    }

    if (false == m_bFix)
        return;

    if (m_bFix)
    {
        Mouse_Move();
        Mouse_Fix();
    }

}

void CPlayer::Mouse_Move()
{
    _long dwMouseMove(0);

    if (dwMouseMove = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_X))
    {
        m_pTransformCom->Rotation(ROT_Y, dwMouseMove / 10.f);
    }

}


void CPlayer::Mouse_Fix()
{
    POINT			ptMouseCenter{ WINCX >> 1, WINCY >> 1 };

    ClientToScreen(g_hWnd, &ptMouseCenter);
    SetCursorPos(ptMouseCenter.x, ptMouseCenter.y);
}


void CPlayer::Set_OnTerrain(const _float& fTimeDelta)
{
    _vec3   vPos = m_pTransformCom->m_vInfo[INFO_POS];


    CTerrainTex* pTerrainBufferCom = dynamic_cast<CTerrainTex*>
        (CManagement::GetInstance()->Get_Component(ID_STATIC, L"GameLogic_Layer", L"Terrain", L"Com_Buffer"));

    if (nullptr == pTerrainBufferCom)
        return;

    _float  fY = m_pCalculatorCom->Compute_HeightOnTerrain(&vPos, pTerrainBufferCom->Get_VtxPos());

    if (vPos.y < fY + 1.f) // KEY_INPUT에서의 이동에 따른 높이 보정
    {
        vPos.y = fY + 1.f;
        m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
    }
    else if ((m_iJumpState == JUMP_NOT) && (vPos.y > fY + 1.f)) // 가파른 내리막길 또는 절벽에서 자유낙하
    {
        _float fClampDelta = 0.05f; // 작은 경사에서 자유낙하하는 대신 지면클램핑 시킬 범위
        if (vPos.y - (fY + 1.f) < fClampDelta)
        {
            vPos.y = fY + 1.f;
            m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z); // 지면클램핑
        }
        else
        {
            m_iJumpState = JUMP_FREEFALL;
            m_fJumpTime = 0.f;
        }
    }

    _float fJumpSpeed = 30.f;
    _float fYDelta;

#pragma region fYDelta 공식 유도과정

    // PARABOLIC(포물선 점프 상태의 fYDelta)

    // fYDelta  = fYInitial + fJumpSpeed * (m_fJumpTime + fTimeDelta) - 1/2  * GRAVCONST * (fJumpTime + fTimeDelta)*(fJumpTime + fTimeDelta)
    // - (fYInitial + fJumpSpeed * m_fJumpTime - 1/2 * GRAVCONST * m_fJumpTime * m_fJumpTime )
    // 
    // = fJumpSpeed * fTimeDelta  - 1/2 * GRAVCONST *  ((m_fJumpTime + fTimeDelta)*(m_fJumpTime + fTimeDelta) - m_fJumpTime * m_fJumpTime)
    // 
    // = fJumpSpeed * fTimeDelta - 1/2 * GRAVCONST * ( 2 * m_fJumpTime * fTimeDelta + fTimeDelta * fTimeDelta )
    // 
    // = fTimeDelta * (fJumpSpeed - 1/2 * GRAVCONST (2* m_fJumpTime + fTimeDelta));

    // FREEFALL(자유낙하 상태의 fYDelta)

    //fYDelta = -(1 / 2 * GRAVCONST * (m_fJumpTime + fTimeDelta) * (m_fJumpTime + fTimeDelta) - 1 / 2 * GRAVCONST * m_fJumpTime * m_fJumpTime)
    //
    //= -(1 / 2 * GRAVCONST * (2 * m_fJumpTime * fTimeDelta + fTimeDelta * fTimeDelta)
    //
    //= -(1 / 2 * GRAVCONST * fTimeDelta * (2 * m_fJumpTime + fTimeDelta);

#pragma endregion

    if (m_iJumpState == JUMP_PARABOLIC) // 포물선 점프중일때 
    {
        fYDelta = fTimeDelta * (fJumpSpeed - 0.5f * GRAVCONST * (2 * m_fJumpTime + fTimeDelta));
        vPos.y += fYDelta;
        m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
    }
    else if (m_iJumpState == JUMP_FREEFALL) // 자유낙하 상태일때
    {
        fYDelta = fTimeDelta * (-0.5f * GRAVCONST * (2 * m_fJumpTime + fTimeDelta));
        vPos.y += fYDelta;
        m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
    }

    fY = m_pCalculatorCom->Compute_HeightOnTerrain(&vPos, pTerrainBufferCom->Get_VtxPos());
    if (vPos.y < fY + 1.f) // 바닥에 착지
    {
        m_pTransformCom->Set_Pos(vPos.x, fY + 1.f, vPos.z);
        m_iJumpState = JUMP_NOT;
        m_fJumpTime = 0.f;
    }

    m_fJumpTime += fTimeDelta;
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
