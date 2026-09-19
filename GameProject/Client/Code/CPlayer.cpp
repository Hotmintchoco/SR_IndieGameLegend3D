#include "pch.h"
#include "CPlayer.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CDInputMgr.h"
#include "CTerrain.h"
#include "CSphereCollider.h"
#include "CCollisionMgr.h"
#include "CImGuiTool.h"
#include "CCameraMgr.h"
#include "CGameStatusMgr.h"
#include "CGun.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev), m_bFix(true), m_bCheck(true), m_iHP(12), m_iMaxHP(12)
{
}


CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    ::ShowCursor(FALSE);

	__super::Ready_GameObject();

    m_pColliderCom->Set_Radius(0.75f);
	m_pColliderCom->Set_CollisionID(COLL_PLAYER);

	m_pTransformCom->Set_Pos(60.f, 1.f, 60.f);

    return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
    _vec3   vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    Compute_ViewZ(&vPos);

    Key_Input(fTimeDelta);

    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    /* 성철 : 매니저 객체로 게임 상태를 관리하기 위해 추가. 문제 발생 시 말해줘 */
    CGameStatusMgr::GetInstance()->UpdatePlayerPosition(vPos);
    /* ---------------------------------------------------------------- */

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

#ifdef _DEBUG
     //RenderImGui();
#endif
}

void CPlayer::RenderImGui()
{
    /* ImGui */
    ImGui::Begin("Player Debug Information");

    /* 위치 */
    _vec3 vPlayerPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPlayerPos);
    ImGui::Text("Pos : %.2f, %.2f, %.2f", vPlayerPos.x, vPlayerPos.y, vPlayerPos.z);

    /* 상하좌우 키 입력 */

    char cKeyStateQ = ' ';
    char cKeyStateW = ' ';
    char cKeyStateA = ' ';
    char cKeyStateS = ' ';
    char cKeyStateD = ' ';
    char cKeyStateF = ' ';
    char cKeyStateC = ' ';
    const char* cKeyStateShift = "     ";
    if (CDInputMgr::GetInstance()->Key_Press(DIK_Q)) cKeyStateQ = 'Q';
    if (CDInputMgr::GetInstance()->Key_Press(DIK_W)) cKeyStateW = 'W';
    if (CDInputMgr::GetInstance()->Key_Press(DIK_A)) cKeyStateA = 'A';
    if (CDInputMgr::GetInstance()->Key_Press(DIK_S)) cKeyStateS = 'S';
    if (CDInputMgr::GetInstance()->Key_Press(DIK_D)) cKeyStateD = 'D';
    if (CDInputMgr::GetInstance()->Key_Press(DIK_F)) cKeyStateF = 'F';
    if (CDInputMgr::GetInstance()->Key_Press(DIK_LSHIFT)) cKeyStateShift = "SHIFT";
    if (CDInputMgr::GetInstance()->Key_Press(DIK_C)) cKeyStateC = 'C';


    ImGui::Text("KEY INPUT STATE");
    ImGui::Text("       [%c][%c]", cKeyStateQ, cKeyStateW);
    ImGui::Text("       [%c][%c][%c][%c]", cKeyStateA, cKeyStateS, cKeyStateD, cKeyStateF);
    ImGui::Text("[%s]            [%c]", cKeyStateShift, cKeyStateC);


    /* 카메라 */
    _float fAngle;
    CCameraMgr::GetInstance()->Get_CameraAngle(&fAngle);
    ImGui::Text("Camera Angle : %.2f", fAngle);

    ImGui::End();
}

void CPlayer::OnCollisionEnter(CGameObject* pOther)
{

}

void CPlayer::OnCollisionStay(CGameObject* pOther)
{
    // 장애물과 충돌 시에 마찰력 적용
    CCollider* pOtherCollider = nullptr;
        
	if (nullptr == pOtherCollider)
        pOtherCollider = dynamic_cast<CCollider*>(pOther->Get_Component(ID_DYNAMIC, L"Com_Collider"));
    if (nullptr == pOtherCollider)
        pOtherCollider = dynamic_cast<CCollider*>(pOther->Get_Component(ID_DYNAMIC, L"Com_Collider0"));
    if (nullptr == pOtherCollider)
        pOtherCollider = dynamic_cast<CCollider*>(pOther->Get_Component(ID_DYNAMIC, L"Com_Collider1"));

    if (pOtherCollider && pOtherCollider->Get_CollisionID() == COLL_OBSTACLE)
        m_fFrictionForce = 0.75f; // 마찰력 적용
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

    _float fSpeed = 5.f * m_fFrictionForce;

    if (CDInputMgr::GetInstance()->Key_Press(DIK_LSHIFT))
    {
        fSpeed *= 2;
    }

    _bool bIsMove = false;
    _vec3 vFinalVec = { 0.f, 0.f, 0.f };

    if (CDInputMgr::GetInstance()->Key_Press(DIK_W))
    {
        vFinalVec = vFinalVec + vLook;
    }

    if (CDInputMgr::GetInstance()->Key_Press(DIK_S))
    {
        vFinalVec = vFinalVec - vLook;
    }

    if (CDInputMgr::GetInstance()->Key_Press(DIK_A))
    {
        vFinalVec = vFinalVec - vRight;
    }

    if (CDInputMgr::GetInstance()->Key_Press(DIK_D))
    {
        vFinalVec = vFinalVec + vRight;
    }

    if (vFinalVec.x > FLT_EPSILON || vFinalVec.y || FLT_EPSILON && vFinalVec.z || FLT_EPSILON)  m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vFinalVec, &vFinalVec), fSpeed, fTimeDelta);


    if (CDInputMgr::GetInstance()->Key_Down(DIK_TAB))
    {
        m_bFix = !m_bFix;

        if (m_bFix)
        {
            while (::ShowCursor(FALSE) >= 0) {}
        }
        else
        {
            while (::ShowCursor(TRUE) < 0) {}
        }
    }

    if (false == m_bFix)
        return;

    Mouse_Move();
    Mouse_Fix();
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

void CPlayer::GetItem(ITEMID iItemID)
{
    switch (iItemID)
    {
    case ITEM_HEAL : 
        UpdateHP(1);
        break;
    case ITEM_SKILLGAUGE : 
        CGun* pGun = dynamic_cast<CGun*>(CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"Gun"));
        if (pGun != nullptr)
        {
            pGun->UpdateUltimateGauge(0.05f);
            pGun->UpdateSpecialGauge(0.05f);
        }
        break;
    }
}

void CPlayer::UpdateHP(_int iAmount)
{
    if (iAmount > 0)
    {
        if (m_iHP + iAmount > m_iMaxHP)
        {
            CGameStatusMgr::GetInstance()->UpdatePlayerHp(m_iMaxHP - m_iHP);
            m_iHP = m_iMaxHP;

        }
        else
        {
            CGameStatusMgr::GetInstance()->UpdatePlayerHp(iAmount);
            m_iHP += iAmount;
        }
    }
    else if (iAmount < 0)
    {
        if (m_iHP + iAmount <= 0)
        {
            CGameStatusMgr::GetInstance()->UpdatePlayerHp(-m_iHP);
            m_iHP = 0;
            Set_Dead(true);
        }
        else
        {
            CGameStatusMgr::GetInstance()->UpdatePlayerHp(iAmount);
            m_iHP += iAmount;
        }
    }
}

void CPlayer::Free()
{
    CGameObject::Free();
}