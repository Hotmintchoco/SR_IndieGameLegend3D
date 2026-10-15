#include "pch.h"
#include "CPlayerMovement.h"
#include "CTransform.h"

CPlayerMovement::CPlayerMovement(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMovement(pGraphicDev)
{
    m_fMaxGroundSpeed = m_fMaxGroundSpeedRaw;
    m_fMaxAirSpeedH = m_fMaxAirSpeedHRaw;
    m_fMaxAirSpeedV = m_fMaxAirSpeedVRaw;
}

CPlayerMovement::~CPlayerMovement()
{
}

_int CPlayerMovement::Update_Component(_float fTimeDelta)
{
    _int iExit = CMovement::Update_Component(fTimeDelta);

    if (m_bOnSlipping)
    {
        m_fLeftSlipTime -= fTimeDelta;
        
        if (m_fLeftSlipTime <= 0.f)
        {
            m_bOnSlipping = false;
            /* 	float m_fGroundFriction = 30.f; */
            m_fGroundFriction = 30.f;
            m_fInputAcc = 100.f;
        }
    }

    return iExit;
}

void CPlayerMovement::LateUpdate_Component()
{
    CMovement::LateUpdate_Component();
}

void CPlayerMovement::SetSpeedScale(float fScale)
{
    m_fSpeedScale = fScale;

    m_fMaxGroundSpeed = m_fMaxGroundSpeedRaw * fScale * m_fHorizontalSpeedCoef;
    m_fMaxAirSpeedH = m_fMaxAirSpeedHRaw * fScale * m_fHorizontalSpeedCoef;
    m_fMaxAirSpeedV = m_fMaxAirSpeedVRaw * fScale;
    m_fJumpSpeed = m_fJumpSpeedRaw * fScale;
    m_fGravity = m_fGravityRaw * fScale;
    m_fSkin = m_fSkinRaw * fScale;
}

void CPlayerMovement::SetHorizontalSpeedCoef(float fCoef)
{
    m_fHorizontalSpeedCoef = fCoef;

    /* 현재 가짜 스케일 기준으로 다시 계산 */
    SetSpeedScale(m_fSpeedScale);
}

void CPlayerMovement::Knockback(const _vec3& vDir, float fIntensity)
{
    SetSprint(false);

    _vec3 vNormDirFlat = {vDir.x, 0.f, vDir.z};
    D3DXVec3Normalize(&vNormDirFlat, &vNormDirFlat);
    _vec3 vWorldUp{ 0.f, 1.f, 0.f };
    _vec3 vRight;
    D3DXVec3Cross(&vRight, &vNormDirFlat, &vWorldUp);
    D3DXVec3Normalize(&vRight, &vRight);
    
    _matrix matRot;
    D3DXMatrixRotationAxis(&matRot, &vRight, m_fKnockbackAngle);
    
    _vec3 vRot;
    D3DXVec3TransformNormal(&vRot, &vNormDirFlat, &matRot);

    TLaunchRequest t{};
    t.bOverrideH = true;
    t.bOverrideV = false;
    t.vVelocity = vRot * fIntensity;
    t.fAlpha = 0.2f;

    Launch(t);
}

void CPlayerMovement::Walk(const _vec2& vCommand)
{
    _vec2 vNormCommand;
    D3DXVec2Normalize(&vNormCommand, &vCommand);

    _vec3 vLook{ sinf(m_fRefYaw), 0.f, cosf(m_fRefYaw) };
    _vec3 vRight{ cosf(m_fRefYaw), 0.f, -sinf(m_fRefYaw) };

    _vec3 vDir = vNormCommand.x * vRight + vNormCommand.y * vLook;

    m_vAccCommand = vDir * ((!m_bOnGround) ? m_fAirInputCoef : 1.f);
}

void CPlayerMovement::Stop()
{
    m_vVelocity = _vec3{ 0.f, 0.f, 0.f };
}

void CPlayerMovement::Jump()
{
    if (!m_bOnGround) return;

    m_bOnGround = false;
    AddImpulse(_vec3{0.f, 1.f, 0.f}, m_fJumpSpeed);
}

void CPlayerMovement::Slip(float fTime)
{
    m_bOnSlipping = true;
    m_fLeftSlipTime = max(m_fLeftSlipTime, fTime);
    m_fGroundFriction = 2.f;
    m_fInputAcc = 8.f;
}

float CPlayerMovement::GetCurMaxGroundSpeed() const
{
    return m_fMaxGroundSpeed * (m_bSprinting ? m_fSprintCoef : 1.f);

}

CPlayerMovement* CPlayerMovement::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    return new CPlayerMovement(pGraphicDev);
}

void CPlayerMovement::Free()
{
    CMovement::Free();
}
