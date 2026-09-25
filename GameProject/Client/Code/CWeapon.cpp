#include "pch.h"
#include "CWeapon.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CCameraMgr.h"
#include "CImGuiTool.h"
#include "CDefaultBullet.h"
#include "CDInputMgr.h"
#include "CGameStatusMgr.h"
#include "CSoundMgr.h"
#include "Client_Struct.h"
#include "CRoomLayer.h"

CWeapon::CWeapon(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CWeapon::~CWeapon()
{
}

HRESULT CWeapon::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CWeapon::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
    
    Animation(fTimeDelta);

    CheckCoolTime(fTimeDelta);

    return iExit;
}

void CWeapon::CheckCoolTime(const _float& fTimeDelta)
{
    if (m_bIsCoolTime)
    {
        m_fCoolTimeLeft -= fTimeDelta;

        if (m_fCoolTimeLeft <= 0.f)
        {
            m_bIsCoolTime = false;
            m_fCoolTimeLeft = 0.f;
        }
    }
}

void CWeapon::LateUpdate_GameObject(const _float& fTimeDelta)
{
    SyncTransformToCamera();

    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CWeapon::SyncTransformToCamera()
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

void CWeapon::DefaultAttack()
{
    _vec3 vDir = m_vBulletTo - m_vBulletFrom;

    CProjectile* pProjectile = CDefaultBullet::Create(m_pGraphicDev, m_vBulletFrom, vDir);
    CGameStatusMgr::GetInstance()->GetCurrentRoomLayer()->Add_GameObject(L"Projectile_" + to_wstring(pProjectile->GetProjectileID()), pProjectile);

    CSoundMgr::GetInstance()->PlaySFX(L"sfxBullet.wav");

    m_bIsCoolTime = true;
    m_fCoolTimeLeft = m_fShootInterval;
    StartShotAnimation();
}

void CWeapon::UpdateAnimationArgs(const TWeaponAnimArgs& t)
{
    m_bOnSprint = t.bSprint;
    m_bOnMoveAnimation = t.bMove;
    m_bSpecialAttackSwitchOn = t.bSpecialAtk;
}


void CWeapon::UpdateLocalTransform(const _vec3& vScale, const _vec3& vRotation, const _vec3& vTransition)
{
    if (!m_pTransformCom) return;

    m_pTransformCom->Set_Scale(vScale);
    m_pTransformCom->Set_Rotation_Raw(vRotation);
    m_pTransformCom->Set_Pos(vTransition);
}

void CWeapon::Animation(const _float fTimeDelta)
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

void CWeapon::StartShotAnimation()
{
    m_bShotAnimation = true;
    m_fTimeAfterShot = 0.f;
}

HRESULT CWeapon::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    return S_OK;
}

void CWeapon::Free()
{
    CGameObject::Free();
}