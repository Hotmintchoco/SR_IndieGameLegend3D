#include "pch.h"
#include "CPlayer_MG1.h"
#include "CProtoMgr.h"
#include "CTimerMgr.h"
#include "CStage.h"
#include "CManagement.h"
#include "CPlayerAnimator.h"
#include "CMovement.h"
#include "CPlayerPartTex.h"
#include "CDInputMgr.h"
#include "CPlayerMovement.h"
#include "CCursorPolicyMgr.h"
#include "CUIMgr.h"
#include "CCollisionMgr.h"
#include "CWeaponSystem.h"
#include "Client_Struct.h"
#include "CClientCameraMgr.h"
#include "CCamera_MG1.h"
#include "CCamera2_MG1.h"
#include "CBox_MG1.h"

CPlayer_MG1::CPlayer_MG1(LPDIRECT3DDEVICE9 pGraphicDev)
    :CGameObject(pGraphicDev)
{
}

CPlayer_MG1::~CPlayer_MG1()
{
}

HRESULT CPlayer_MG1::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    m_pColliderCom->Set_Radius(0.5f);

    m_pColliderCom->Set_CollisionID(COLL_PLAYER);

    _vec3 vPos{ 0.f,0.f,0.f };
    m_pTransformCom->Set_Pos(vPos);

    //_vec3 vScale{ 0.25f,0.25f,0.25f };
    _vec3 vScale{ 0.5f,0.5f,0.5f };
    m_pTransformCom->Set_Scale(vScale);
    m_vOriginScale = vScale;
    m_fYaw = D3DXToRadian(-90.f);

    return S_OK;
}

_int CPlayer_MG1::Update_GameObject(_float fTimeDelta)
{

    int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CCollisionMgr::GetInstance()->Add_Collider(COLL_PLAYER, m_pColliderCom);

    if (m_bConnection == true || m_bConnection2 == true)
    {
        CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
    }
    else
    {
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);
    }

    Update_Input(fTimeDelta);
    Check_Connection();
    Check_Connection2();

    _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
    if (!((-14.5f < vPos.x) && (vPos.x < -0.5f) && (2.5f < vPos.z)  && (vPos.z < 5.5f))
        && (1.f < vPos.y))
    {
        m_bJumpState = true;
        m_bJumpState2 = true;
    }
    if (!((-14.5f < vPos.x) && (vPos.x < -9.5f) && (2.5f < vPos.z) && (vPos.z < 5.5f))
        && (3.f < vPos.y))
    {
        m_bJumpState = true;
        m_bJumpState2 = true;
    }
    if(-0.1f < vPos.y && vPos.y < 0.1f)
		m_pTransformCom->Set_Scale(m_vOriginScale);

    if (1.9f < vPos.y && vPos.y < 2.1f)
    {
        _vec3 vScale = { 0.35f,0.35f ,0.35f };
        m_pTransformCom->Set_Scale(vScale);
    }
    Jump(fTimeDelta);

    if (m_iCameraType == 1)
    {
        if (((-15.5f < vPos.x) && (vPos.x < -0.5f) && (2.5f < vPos.z) && (vPos.z < 5.5f))
            && (0.9f < vPos.y && vPos.y < 1.3f))
        {
            _vec3 vScale = { 0.35f, 0.35f, 0.35f };
            m_pTransformCom->Set_Scale(vScale);
            _vec3 vPos2 = vPos;
            vPos2.y = 1.35f;
            m_pTransformCom->Set_Pos(vPos2);
        }
        //if (3.f < vPos.y && vPos.y < 3.4f)
        //{
        //    _vec3 vPos2 = vPos;
        //    vPos2.y = 3.5f;
        //    m_pTransformCom->Set_Pos(vPos2);
        //}
    }
    return iExit;
}

void CPlayer_MG1::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CPlayer_MG1::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture(0);
    m_pBufferCom2->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CPlayer_MG1::Add_Component()
{
    CComponent* pComponent = nullptr;

    // 임시 RcCol ///////////////////
    pComponent = m_pBufferCom2 = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));
    if (nullptr == pComponent) return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });
    // 임시 //////////////////////////
    
    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Collider
    pComponent = m_pColliderCom = dynamic_cast<CCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SphereCollider"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    // Texture
    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Player_Texture"));
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_speyederTexture"));
    if (nullptr == pComponent)
        return E_FAIL;
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CPlayer_MG1::Update_Input(const _float& fTimeDelta)
{
    if (CDInputMgr::GetInstance()->Key_Press(DIK_W))
    {
        _vec3 vDir; m_pTransformCom->Get_Info(INFO_LOOK, &vDir);
        vDir.y = 0;
        D3DXVec3Normalize(&vDir, &vDir);
        m_pTransformCom->Move_Pos(&vDir, 3.f, fTimeDelta);
    }
    if (CDInputMgr::GetInstance()->Key_Press(DIK_S))
    {
        _vec3 vDir; m_pTransformCom->Get_Info(INFO_LOOK, &vDir);
        vDir.y = 0;
        vDir *= -1.f;
        D3DXVec3Normalize(&vDir, &vDir);
        m_pTransformCom->Move_Pos(&vDir, 3.f, fTimeDelta);

    }
    if (CDInputMgr::GetInstance()->Key_Press(DIK_D))
    {
        _vec3 vDir; m_pTransformCom->Get_Info(INFO_RIGHT, &vDir);
        vDir.y = 0;
        D3DXVec3Normalize(&vDir, &vDir);
        m_pTransformCom->Move_Pos(&vDir, 3.f, fTimeDelta);
    }
    if (CDInputMgr::GetInstance()->Key_Press(DIK_A))
    {
        _vec3 vDir; m_pTransformCom->Get_Info(INFO_RIGHT, &vDir);
        vDir.y = 0;
        vDir *= -1.f;
        D3DXVec3Normalize(&vDir, &vDir);
        m_pTransformCom->Move_Pos(&vDir, 3.f, fTimeDelta);
    }
    if (CDInputMgr::GetInstance()->Key_Press(DIK_SPACE))
    {
        m_bJumpState = true;
    }
    if (CDInputMgr::GetInstance()->Key_Down(DIK_1))
    {
        if (m_iCameraType == 2)
        {
            m_iCameraType = 1;
            m_pCamera1->Set_IsActive(true);
            m_pCamera2->Set_IsActive(false);
        }
    }
    else if (CDInputMgr::GetInstance()->Key_Down(DIK_2))
    {
        if (m_iCameraType == 1)
        {
            m_iCameraType = 2;
            m_pCamera1->Set_IsActive(false);
            m_pCamera2->Set_IsActive(true);
            static_cast<CCamera2_MG1*>(m_pCamera2)->Set_CameraActive(m_pTransformCom);
        }
    }

    if (m_iCameraType == 2)return;

    if (!CCursorPolicyMgr::GetInstance()->IsCursorFixed()) return;

    _float fMinPitch = D3DXToRadian(-89.f);
    _float fMaxPitch = D3DXToRadian(89.f);

    const _long mouseX = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_X);
    const _long mouseY = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Y);

    _float fDeltaYaw = D3DXToRadian(mouseX / 10.f);
    _float fDeltaPitch = D3DXToRadian(mouseY / 10.f);

    if (!std::isfinite(fDeltaYaw) || !std::isfinite(fDeltaPitch)) return;
    m_fYaw = fmodf(m_fYaw + fDeltaYaw, 2.f * D3DX_PI);
    m_fPitch = max(fMinPitch, min(m_fPitch + fDeltaPitch, fMaxPitch));

    _vec3 fAngle = m_pTransformCom->Get_Angle();

    fAngle.x = D3DXToDegree(m_fPitch);
    fAngle.y = D3DXToDegree(m_fYaw);

    m_pTransformCom->Set_Angle(fAngle);
}

void CPlayer_MG1::Check_Connection()
{
    CTransform* pBoxTransformCom1 = dynamic_cast<CTransform*>(m_pBox1->Get_Component(ID_DYNAMIC, L"Com_Transform"));
    CTransform* pBoxTransformCom2 = dynamic_cast<CTransform*>(m_pBox2->Get_Component(ID_DYNAMIC, L"Com_Transform"));

    _vec3 temp1, temp2;
    pBoxTransformCom1->Get_Info(INFO_POS, &temp1);
    pBoxTransformCom2->Get_Info(INFO_POS, &temp2);
    if (temp1.y > temp2.y)
    {
        CTransform* pTemp = pBoxTransformCom1;
        pBoxTransformCom1 = pBoxTransformCom2;
        pBoxTransformCom2 = pTemp;
        CBox_MG1* pTempBox = m_pBox1;
        m_pBox1 = m_pBox2;
        m_pBox2 = pTempBox;
    }

    //vPos2가 높도록
    _vec3 vPosBox1, vPosBox2, vRightBox1, vRightBox2, vScale1, vScale2;
    pBoxTransformCom1->Get_Info(INFO_POS, &vPosBox1);
    pBoxTransformCom2->Get_Info(INFO_POS, &vPosBox2);
    pBoxTransformCom1->Get_Info(INFO_RIGHT, &vRightBox1);
    pBoxTransformCom2->Get_Info(INFO_RIGHT, &vRightBox2);
    vScale1 = pBoxTransformCom1->Get_Scale();
    vScale2 = pBoxTransformCom2->Get_Scale();
    D3DXVec3Normalize(&vRightBox1, &vRightBox1);
    D3DXVec3Normalize(&vRightBox2, &vRightBox2);
    if (m_iCameraType == 1)
    {
        if (m_bConnection == true)
        {
            _vec3 vDist = m_vBoxOriginPos - vPosBox2;
            if (D3DXVec3Length(&vDist) > 0.1f)
            {
                _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
                _vec3 vDir = vPos - vPosBox2;
                vDir.y = 0.f;
                _vec3 vScale = pBoxTransformCom2->Get_Scale();
                if (-vScale.x <= vDir.x && vDir.x <= vScale.x && -vScale.z <= vDir.z && vDir.z <= vScale.z)
                {

                    vDir *= m_vBoxOriginScale.x / vScale2.x;
                    _vec3 vPos2 = m_vBoxOriginPos + vDir;
                    
                    _vec3 vScale3 = { 0.35f,0.35f ,0.35f };
                    m_pTransformCom->Set_Scale(vScale3);

                    _vec3 vScale = m_pTransformCom->Get_Scale();
                    vPos2.y += vScale.y;
                    m_pTransformCom->Set_Pos(vPos2);
                    
                }
            }
            m_pBox2->Set_Connection(false);
            m_pBox1->Set_Connection(false);
            m_bConnection = false;
            pBoxTransformCom2->Set_Pos(m_vBoxOriginPos);
            pBoxTransformCom2->Set_Scale(m_vBoxOriginScale);
        }
        return;
    }
    if (m_bConnectionStart == false)
    {
        m_bConnectionStart = true;
        m_vBoxOriginPos = vPosBox2;
        m_vBoxOriginScale = vScale2;
        m_vBoxOriginRight = vRightBox2;
    }
    
    _matrix matView, matCameraWorld;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixInverse(&matCameraWorld, nullptr, &matView);
    _vec3 vCameraLook = { matView._13,matView._23,matView._33 };
    _vec3 vCameraPos = { matCameraWorld._41, matCameraWorld._42, matCameraWorld._43 };

    _vec3 vPosBox1Point = vPosBox1 - vRightBox1 * vScale1.x;
    _vec3 vPosBox2Point = m_vBoxOriginPos + m_vBoxOriginRight * (m_vBoxOriginScale.x);

    _vec3 vDir = vCameraPos - vPosBox1Point;
    vDir /= vDir.y;

    vDir *= (m_vBoxOriginPos.y - vPosBox1.y);

    vPosBox1Point += vDir;

    _vec3 vDist = vPosBox2Point - vPosBox1Point;

    if (fabsf(vDist.x) < 0.02f && (3.8f < vCameraPos.z && vCameraPos.z < 4.2f))
    {
        m_bConnection = true;
        m_pBox2->Set_Connection(true);
        m_pBox1->Set_Connection(true);

        _vec3 vPos1 = m_vBoxOriginPos - vCameraPos;
        _vec3 vPos2 = m_vBoxOriginPos - vCameraPos - vDir;
        _float fOldDepth = D3DXVec3Dot(&vPos1, &vCameraLook);
        _float fNewDepth = D3DXVec3Dot(&vPos2, &vCameraLook);
        _float fScaleRatio = fNewDepth / fOldDepth;

        _vec3 vEpsilon = { (fScaleRatio - 1.f) * (m_vBoxOriginScale.x),0.f,0.f };
        pBoxTransformCom2->Set_Pos(m_vBoxOriginPos - vDir - vEpsilon);
        pBoxTransformCom2->Set_Scale(m_vBoxOriginScale * fScaleRatio);
    }
    else
    {
        _vec3 vDist = m_vBoxOriginPos - vPosBox2;
        if (D3DXVec3Length(&vDist) > 0.1f)
        {
            _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
            _vec3 vDir = vPos - vPosBox2;
            vDir.y = 0.f;
            _vec3 vScale = pBoxTransformCom2->Get_Scale();
            if (-vScale.x <= vDir.x && vDir.x <= vScale.x && -vScale.z <= vDir.z && vDir.z <= vScale.z)
            {
                vDir *= m_vBoxOriginScale.x / vScale2.x;
                _vec3 vPos2 = m_vBoxOriginPos + vDir;
                _vec3 vScale = m_pTransformCom->Get_Scale();
                //vPos2.y += vScale.y;
                //m_pTransformCom->Set_Pos(vPos2);

                _matrix matView, matCameraWorld;
                m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
                D3DXMatrixInverse(&matCameraWorld, nullptr, &matView);
                _vec3 vEye = { matCameraWorld._41, matCameraWorld._42, matCameraWorld._43 };

                _vec3 vDist = vEye - vPos;
                _vec3 vDist2 = vEye - vPos2;
                _vec3 vScale2 = vScale * (D3DXVec3Length(&vDist2) / D3DXVec3Length(&vDist));
                m_pTransformCom->Set_Scale(vScale2);


                vPos2.y += vScale2.y;
                m_pTransformCom->Set_Pos(vPos2);
            }
        }
        m_pBox2->Set_Connection(false);
        m_pBox1->Set_Connection(false);
        m_bConnection = false;
        pBoxTransformCom2->Set_Pos(m_vBoxOriginPos);
        pBoxTransformCom2->Set_Scale(m_vBoxOriginScale);
    }
}

void CPlayer_MG1::Check_Connection2()
{
    CTransform* pBoxTransformCom1 = dynamic_cast<CTransform*>(m_pBox3->Get_Component(ID_DYNAMIC, L"Com_Transform"));
    CTransform* pBoxTransformCom2 = dynamic_cast<CTransform*>(m_pBox4->Get_Component(ID_DYNAMIC, L"Com_Transform"));

    _vec3 temp1, temp2;
    pBoxTransformCom1->Get_Info(INFO_POS, &temp1);
    pBoxTransformCom2->Get_Info(INFO_POS, &temp2);
    if (temp1.y > temp2.y)
    {
        CTransform* pTemp = pBoxTransformCom1;
        pBoxTransformCom1 = pBoxTransformCom2;
        pBoxTransformCom2 = pTemp;
        CBox_MG1* pTempBox = m_pBox3;
        m_pBox3 = m_pBox4;
        m_pBox4 = pTempBox;
    }

    //vPos2가 높도록
    _vec3 vPosBox1, vPosBox2, vRightBox1, vRightBox2, vScale1, vScale2;
    pBoxTransformCom1->Get_Info(INFO_POS, &vPosBox1);
    pBoxTransformCom2->Get_Info(INFO_POS, &vPosBox2);
    pBoxTransformCom1->Get_Info(INFO_RIGHT, &vRightBox1);
    pBoxTransformCom2->Get_Info(INFO_RIGHT, &vRightBox2);
    vScale1 = pBoxTransformCom1->Get_Scale();
    vScale2 = pBoxTransformCom2->Get_Scale();
    D3DXVec3Normalize(&vRightBox1, &vRightBox1);
    D3DXVec3Normalize(&vRightBox2, &vRightBox2);
    if (m_iCameraType == 1)
    {
        if (m_bConnection2 == true)
        {
            _vec3 vDist = m_vBoxOriginPos2 - vPosBox2;
            if (D3DXVec3Length(&vDist) > 0.1f)
            {
                _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
                _vec3 vDir = vPos - vPosBox2;
                vDir.y = 0.f;
                _vec3 vScale = pBoxTransformCom2->Get_Scale();
                if (-vScale.x <= vDir.x && vDir.x <= vScale.x && -vScale.z <= vDir.z && vDir.z <= vScale.z)
                {

                    vDir *= m_vBoxOriginScale2.x / vScale2.x;
                    _vec3 vPos2 = m_vBoxOriginPos2 + vDir;
                    //m_pTransformCom->Set_Scale(m_vOriginScale);
                    _vec3 vScale = m_pTransformCom->Get_Scale();
                    vPos2.y += vScale.y;
                    m_pTransformCom->Set_Pos(vPos2);

                }
            }
            m_pBox4->Set_Connection(false);
            m_pBox3->Set_Connection(false);
            m_bConnection2 = false;
            pBoxTransformCom2->Set_Pos(m_vBoxOriginPos2);
            pBoxTransformCom2->Set_Scale(m_vBoxOriginScale2);
        }
        return;
    }
    if (m_bConnectionStart2 == false)
    {
        m_bConnectionStart2 = true;
        m_vBoxOriginPos2 = vPosBox2;
        m_vBoxOriginScale2 = vScale2;
        m_vBoxOriginRight2 = vRightBox2;
    }

    _matrix matView, matCameraWorld;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixInverse(&matCameraWorld, nullptr, &matView);
    _vec3 vCameraLook = { matView._13,matView._23,matView._33 };
    _vec3 vCameraPos = { matCameraWorld._41, matCameraWorld._42, matCameraWorld._43 };

    _vec3 vPosBox1Point = vPosBox1 - vRightBox1 * vScale1.x;
    _vec3 vPosBox2Point = m_vBoxOriginPos2 + m_vBoxOriginRight2 * (m_vBoxOriginScale2.x);

    _vec3 vDir = vCameraPos - vPosBox1Point;
    vDir /= vDir.y;

    vDir *= (m_vBoxOriginPos2.y - vPosBox1.y);

    vPosBox1Point += vDir;

    _vec3 vDist = vPosBox2Point - vPosBox1Point;

    if (fabsf(vDist.x) < 0.02f && (3.8f < vCameraPos.z && vCameraPos.z < 4.2f))
    {
        m_bConnection2 = true;
        m_pBox4->Set_Connection(true);
        m_pBox3->Set_Connection(true);

        _vec3 vPos1 = m_vBoxOriginPos2 - vCameraPos;
        _vec3 vPos2 = m_vBoxOriginPos2 - vCameraPos - vDir;
        _float fOldDepth = D3DXVec3Dot(&vPos1, &vCameraLook);
        _float fNewDepth = D3DXVec3Dot(&vPos2, &vCameraLook);
        _float fScaleRatio = fNewDepth / fOldDepth;

        _vec3 vEpsilon = { (fScaleRatio - 1.f) * (m_vBoxOriginScale2.x),0.f,0.f };
        pBoxTransformCom2->Set_Pos(m_vBoxOriginPos2 - vDir - vEpsilon);
        pBoxTransformCom2->Set_Scale(m_vBoxOriginScale2 * fScaleRatio);
    }
    else
    {
        _vec3 vDist = m_vBoxOriginPos2 - vPosBox2;
        if (D3DXVec3Length(&vDist) > 0.1f)
        {
            _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
            _vec3 vDir = vPos - vPosBox2;
            vDir.y = 0.f;
            _vec3 vScale = pBoxTransformCom2->Get_Scale();
            if (-vScale.x <= vDir.x && vDir.x <= vScale.x && -vScale.z <= vDir.z && vDir.z <= vScale.z)
            {
                vDir *= m_vBoxOriginScale2.x / vScale2.x;
                _vec3 vPos2 = m_vBoxOriginPos2 + vDir;
                _vec3 vScale = m_pTransformCom->Get_Scale();
                //vPos2.y += vScale.y;
                //m_pTransformCom->Set_Pos(vPos2);

                _matrix matView, matCameraWorld;
                m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
                D3DXMatrixInverse(&matCameraWorld, nullptr, &matView);
                _vec3 vEye = { matCameraWorld._41, matCameraWorld._42, matCameraWorld._43 };

                _vec3 vDist = vEye - vPos;
                _vec3 vDist2 = vEye - vPos2;
                _vec3 vScale2 = vScale * (D3DXVec3Length(&vDist2) / D3DXVec3Length(&vDist));
                m_pTransformCom->Set_Scale(vScale2);


                vPos2.y += vScale2.y;
                m_pTransformCom->Set_Pos(vPos2);
            }
        }
        m_pBox4->Set_Connection(false);
        m_pBox3->Set_Connection(false);
        m_bConnection2 = false;
        pBoxTransformCom2->Set_Pos(m_vBoxOriginPos2);
        pBoxTransformCom2->Set_Scale(m_vBoxOriginScale2);
    }
}

void CPlayer_MG1::OnCollisionEnter(COLLINFO eCollInfo)
{
}

void CPlayer_MG1::OnCollisionStay(COLLINFO eCollInfo)
{
}

CPlayer_MG1* CPlayer_MG1::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CPlayer_MG1* pObject = new CPlayer_MG1(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CPlayer_MG1 Create Failed");
        return nullptr;
    }

    return pObject;
}

void CPlayer_MG1::Free()
{
    CGameObject::Free();
}

void CPlayer_MG1::Jump(const _float& fTimeDelta)
{
    if (m_bJumpState == true)
    {
        if (m_bJumpStart == false)
        {
            m_bJumpStart = true;
            if (m_bJumpState2 == false)
            {
				m_vJumpDirection = { 0.f,2.5f,0.f };
            }
            else
            {
                m_vJumpDirection = { 0.f,0.f,0.f };
            }
        }
        _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
        _vec3 vScale = m_pTransformCom->Get_Scale();

        m_vJumpDirection.y -= 9.8f * fTimeDelta;

        m_pTransformCom->Move_Pos(&m_vJumpDirection, 1.f, fTimeDelta);

        _float fY = -1.f;

        if ((-14.5f < vPos.x && vPos.x < -0.5f) &&
            (2.5f < vPos.z && vPos.z < 5.5f) &&
            1.f < vPos.y)
        {
            fY = 1.f;
        }

        if ((-14.5f < vPos.x && vPos.x < -9.5f) &&
            (2.5f < vPos.z && vPos.z < 5.5f) &&
            3.f < vPos.y)
        {
            fY = 3.f;
        }

        if (vPos.y < fY + vScale.y)
        {
            vPos.y = fY + vScale.y;
            m_pTransformCom->Set_Pos(vPos);
            m_bJumpStart = false;
            m_bJumpState = false;
            m_bJumpState2 = false;
        }

    }
}