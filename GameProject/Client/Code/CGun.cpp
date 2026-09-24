#include "pch.h"
#include "CGun.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CCameraMgr.h"
#include "CImGuiTool.h"
#include "CBullet.h"
#include "CDInputMgr.h"
#include "CGameStatusMgr.h"

CGun::CGun(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CGun::~CGun()
{
}

HRESULT CGun::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    UpdateLocalTransform(m_vScaleLocal, m_vRotationLocal, m_vPositionLocal);

    return S_OK;
}

_int CGun::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    GetKeyInput();
    
    Animation(fTimeDelta);

    if (m_bIsCoolTime)
    {
        m_fCoolTimeLeft -= fTimeDelta;

        if (m_fCoolTimeLeft <= 0.f)
        {
            m_bIsCoolTime = false;
            m_fCoolTimeLeft = 0.f;
        }
    }

    return iExit;
}

void CGun::GetKeyInput()
{
    if (CDInputMgr::GetInstance()->Mouse_Press(DIM_LB))
    {
        TryShoot();
    }

    if (CDInputMgr::GetInstance()->Key_Down(DIK_F))
    {
        m_bSpecialAttackSwitchOn = !m_bSpecialAttackSwitchOn;
    }

    if (CDInputMgr::GetInstance()->Key_Down(DIK_C))
    {
        if (m_bIsUltimateAttackReady)
        {
            UltimateAttack();
        }
    }

    if (CDInputMgr::GetInstance()->Key_Press(DIK_LSHIFT))
    {
        m_bOnSprint = true;
    }
    else
    {
        m_bOnSprint = false;
    }

    if (CDInputMgr::GetInstance()->Key_Press(DIK_W)
        || CDInputMgr::GetInstance()->Key_Press(DIK_A)
        || CDInputMgr::GetInstance()->Key_Press(DIK_S)
        || CDInputMgr::GetInstance()->Key_Press(DIK_D))
    {
        m_bOnMoveAnimation = true;
    }
    else
    {
        m_bOnMoveAnimation = false;
    }
}

void CGun::LateUpdate_GameObject(const _float& fTimeDelta)
{
    SyncTransformToCamera();

    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CGun::SyncTransformToCamera()
{
    /* 카메라 위치를 받아 위치값 조정*/
    _matrix matCamera, matWorld;
    CCameraMgr::GetInstance()->GetCamera(L"Camera_Player_FPV")->GetWorld(&matCamera);
    D3DXMatrixMultiply(&matWorld, m_pTransformCom->Get_World(), &matCamera);
    m_pTransformCom->Set_World(&matWorld);
    
    /* 총구 위치와 발사 방향 업데이트 */
    D3DXVec3TransformCoord(&m_vBulletFrom, &m_vMuzzlePositionLocal, &matWorld);
    _vec3 vCameraLook, vCameraPos;
    memcpy(&vCameraLook, &matCamera.m[2][0], sizeof(_vec3));
    memcpy(&vCameraPos, &matCamera.m[3][0], sizeof(_vec3));
    m_vBulletTo = vCameraPos + vCameraLook * m_fTargetDistance;
}

void CGun::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    if (m_bSpecialAttackSwitchOn)
    {
        m_pTextureCom->Set_Texture(1);
    }
    else
    {
        m_pTextureCom->Set_Texture(0);
    }

    m_pBufferCom->Render_Buffer();

    RenderEditorPanel();
}

void CGun::TryShoot()
{
    if (m_bIsCoolTime) return;

    _vec3 tmp = m_vBulletTo - m_vBulletFrom;

    CGameObject* pGameObject = CBullet::Create(m_pGraphicDev, &m_vBulletFrom, &tmp);
    CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Add_GameObject(L"Bullet", pGameObject);

    m_bIsCoolTime = true;
    m_fCoolTimeLeft = m_fShootInterval;
    StartShotAnimation();
}

void CGun::GainEnergy()
{
    m_fSpecialAtkGauge += 0.1f;
    m_fSpecialAtkGauge = clamp(m_fSpecialAtkGauge, 0.f, 1.f);
    CGameStatusMgr::GetInstance()->SetSpecialAttackGauge(m_fSpecialAtkGauge);

    m_fUltimateAtkGauge += 0.1f;
    m_fUltimateAtkGauge = clamp(m_fUltimateAtkGauge, 0.f, 1.f);
    if (m_fUltimateAtkGauge == 1.f)
    {
        m_bIsUltimateAttackReady = true;
    }
    CGameStatusMgr::GetInstance()->SetUltimateGauge(m_fUltimateAtkGauge);
}

HRESULT CGun::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Mesh
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Gun_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Gun_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    return S_OK;
}

void CGun::RenderEditorPanel()
{
    ImGui::Begin("Gun");

    ImGui::SeparatorText("Transform");
    ImGui::DragFloat3("Scale", &m_vScaleLocal.x, 0.01f, 0.001f, 100.f);
    ImGui::DragFloat3("Position", &m_vPositionLocal.x, 0.01f);
    ImGui::DragFloat3("Rotation", &m_vRotationLocal.x, 0.5f, -360.f, 360.f);

    ImGui::SeparatorText("Animation");
    ImGui::DragFloat("Move Cycle", &m_fMoveAnimationFrequency, 0.01f, 0.05f, 5.f, "%.2f s");
    ImGui::DragFloat("Horizontal Move", &m_fHorizontalMove, 0.001f, 0.f, 1.f);
    ImGui::DragFloat("Quadratic A", &m_fQuadraticA, 0.001f, 0.f, 1.f);
    ImGui::DragFloat("Max Recoil Angle", &m_fMaxRecoilAngle, 0.5f, -90.f, 0.f);
    ImGui::DragFloat("Recoil Damping", &m_fRecoilDamping, 0.05f, 0.f, 20.f);

    ImGui::End();

    UpdateLocalTransform(m_vScaleLocal, m_vRotationLocal, m_vPositionLocal);
}

void CGun::UpdateLocalTransform(const _vec3& vScale, const _vec3& vRotation, const _vec3& vTransition)
{
    if (!m_pTransformCom) return;

    m_pTransformCom->Set_Scale(vScale);
    m_pTransformCom->Set_Rotation_Raw(vRotation);
    m_pTransformCom->Set_Pos(vTransition);
}

void CGun::UltimateAttack()
{
    m_fUltimateAtkGauge = 0.f;
    CGameStatusMgr::GetInstance()->SetUltimateGauge(m_fUltimateAtkGauge);
    m_bIsUltimateAttackReady = false;

    cout << " 궁극기 " << endl;
}

void CGun::Animation(const _float fTimeDelta)
{
    if (m_bShotAnimation)
    {
        m_fTimeAfterShot += fTimeDelta;
        float fT = m_fTimeAfterShot / m_fShootInterval;        
        float fRotXDegree = (expf(-m_fRecoilDamping * fT) - expf(-m_fRecoilDamping)) / (1.f - expf(-m_fRecoilDamping)) * m_fMaxRecoilAngle;

        UpdateLocalTransform(m_vScaleLocal, m_vRotationLocal + _vec3{ fRotXDegree, 0.f, 0.f }, m_vPositionLocal);

        if (m_fTimeAfterShot >= m_fShootInterval)
        {
            m_bShotAnimation = false;
        }
    } 
    else if (m_bOnMoveAnimation)
    {
        float fSprintCoef = (m_bOnSprint) ? 2.f : 1.f;
        m_fTimeAfterMove += fSprintCoef * fTimeDelta;

        float fT = sinf(2 * D3DX_PI * m_fMoveAnimationFrequency * m_fTimeAfterMove);
        _vec2 v{ m_fHorizontalMove * fT, m_fQuadraticA * fT * fT };

        UpdateLocalTransform(m_vScaleLocal, m_vRotationLocal, m_vPositionLocal + _vec3{ v.x, v.y, 0.f });

    }
    else
    {
        m_fTimeAfterMove = 0.f;
    }
}

void CGun::StartShotAnimation()
{
    m_bShotAnimation = true;
    m_fTimeAfterShot = 0.f;
}

CGun* CGun::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CGun* pGun = new CGun(pGraphicDev);

    if (FAILED(pGun->Ready_GameObject()))
    {
        Safe_Release(pGun);
        MSG_BOX("CGun Create Failed");
        return nullptr;
    }

    return pGun;
}

void CGun::Free()
{
    CGameObject::Free();
}