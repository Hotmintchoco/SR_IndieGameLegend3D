#include "pch.h"
#include "CWeapon.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CClientCameraMgr.h"
#include "CPlayerCamera.h"
#include "CImGuiTool.h"
#include "CDefaultBullet.h"
#include "CDInputMgr.h"
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

    m_pTransformCom->SetUseLocal(true);

    m_tLocalFView = { {0.3f, 0.3f, 0.45f}, {-1.f, -2.f, 0.f}, {0.13f, -0.33f, 0.35f} };
    m_tLocalTView = { {0.3f, 0.3f, 0.45f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f} };
    m_vMuzzlePositionLocal = _vec3{ 0.0f, 0.4f, 0.7f };
    UpdateLocalTransform(m_tLocalFView);


    return S_OK;
}

_int CWeapon::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

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

void CWeapon::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    /* 1인칭 업데이트 (3인칭인 경우는 소켓에서 위치 업데이트 해줌) */
    CPlayerCamera* pCamera = dynamic_cast<CPlayerCamera*>(CClientCameraMgr::GetInstance()->Find_Camera(CLIENT_CAMERA_TYPE::PLAYER));
    if (pCamera)
    {
        _matrix matCamWorld;
        pCamera->GetWorld(&matCamWorld);

        switch (pCamera->Get_CameraMode())
        {
        case CAMERA_MODE::FIRST_PERSON:
        {
            SyncTransformToCamera(pCamera);
            Animation(fTimeDelta);
            break;
        }
        case CAMERA_MODE::THIRD_PERSON:
        {
            break;
        }
        }
        UpdateBulletShotPos(*m_pTransformCom->Get_World(), matCamWorld);
    }
}

void CWeapon::SyncTransformToCamera(CCamera* pCamera)
{
    /* 카메라 위치를 받아 위치값 조정*/
    _matrix matCamera;
    pCamera->GetWorld(&matCamera);
    m_pTransformCom->WorldMatrixPropagation(matCamera);
}

void CWeapon::UpdateBulletShotPos(const _matrix& matWorld, const _matrix& matCamera)
{
    /* 총구 위치와 발사 방향 업데이트 */
    D3DXVec3TransformCoord(&m_vBulletFrom, &m_vMuzzlePositionLocal, &matWorld);
    _vec3 vCameraLook, vCameraPos;
    memcpy(&vCameraLook, &matCamera.m[2][0], sizeof(_vec3));
    memcpy(&vCameraPos, &matCamera.m[3][0], sizeof(_vec3));
    m_vBulletTo = vCameraPos + vCameraLook * m_fTargetDistance;
}

void CWeapon::UpdateAnimationArgs(const TWeaponAnimArgs& t)
{
    m_bOnSprint = t.bSprint;
    m_bOnMoveAnimation = t.bMove;
    m_bSpecialAttackSwitchOn = t.bSpecialAtk;
}


void CWeapon::UpdateLocalTransform(const TWeaponLocalInfo& tInfo)
{
    if (!m_pTransformCom) return;

    m_pTransformCom->Set_Scale(tInfo.vScale);
    m_pTransformCom->Set_Rotation_Raw(tInfo.vRotation);
    m_pTransformCom->Set_Pos(tInfo.vPosition);
}

void CWeapon::Animation(const _float fTimeDelta)
{
    if (m_bShotAnimation)
    {
        m_fTimeAfterShot += fTimeDelta;
        float fT = m_fTimeAfterShot / m_fShootInterval;        
        float fRotXDegree = (expf(-m_fRecoilDamping * fT) - expf(-m_fRecoilDamping)) / (1.f - expf(-m_fRecoilDamping)) * m_fMaxRecoilAngle;

        TWeaponLocalInfo t = { m_tLocalFView.vScale, m_tLocalFView.vRotation + _vec3{ fRotXDegree, 0.f, 0.f }, m_tLocalFView.vPosition };

        UpdateLocalTransform(t);

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

        TWeaponLocalInfo t = { m_tLocalFView.vScale, m_tLocalFView.vRotation, m_tLocalFView.vPosition + _vec3{ v.x, v.y, 0.f } };

        UpdateLocalTransform(t);
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

TWeaponOutput CWeapon::DefaultAttack(EInputState ePri, EInputState eSec)
{
    switch (ePri)
    {
    case EInputState::Held:
    {
        ShotSingleBullet();

        m_bIsCoolTime = true;
        m_fCoolTimeLeft = m_fShootInterval;
        return { true, EWeaponAnimEvent::GUN_SHOT };
        break;
    }
    default:
        break;
    }

    return { false, EWeaponAnimEvent::NONE };
}

void CWeapon::ShotSingleBullet()
{
    _vec3 vDir = m_vBulletTo - m_vBulletFrom;
    D3DXVec3Normalize(&vDir, &vDir);

    CProjectile* pProjectile = CDefaultBullet::Create(m_pGraphicDev, m_vBulletFrom, vDir);
    CScene* pScene = CManagement::GetInstance()->GetCurrentScene();
    pScene->Add_GameObject(L"Projectile_" + to_wstring(pProjectile->GetProjectileID()), pProjectile);

    CSoundMgr::GetInstance()->PlaySFX(L"sfxBullet.wav");

    StartShotAnimation();
}

void CWeapon::ShotSingleBullet(const _vec3& vToward)
{
    _vec3 vDir = vToward - m_vBulletFrom;
    D3DXVec3Normalize(&vDir, &vDir);

    CProjectile* pProjectile = CDefaultBullet::Create(m_pGraphicDev, m_vBulletFrom, vDir);
    CScene* pScene = CManagement::GetInstance()->GetCurrentScene();
    pScene->Add_GameObject(L"Projectile_" + to_wstring(pProjectile->GetProjectileID()), pProjectile);

    CSoundMgr::GetInstance()->PlaySFX(L"sfxBullet.wav");

    StartShotAnimation();
}

TWeaponLocalInfo CWeapon::GetLocalInfo(CAMERA_MODE eMode)
{
    return (eMode == CAMERA_MODE::FIRST_PERSON) ? m_tLocalFView : m_tLocalTView;
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
