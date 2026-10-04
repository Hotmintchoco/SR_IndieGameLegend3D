#include "pch.h"
#include "CPlayerMovement.h"
#include "CTransform.h"

CPlayerMovement::CPlayerMovement(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMovement(pGraphicDev)
{
}

CPlayerMovement::~CPlayerMovement()
{
}

_int CPlayerMovement::Update_Component(_float fTimeDelta)
{
    _int iExit = CMovement::Update_Component(fTimeDelta);

    return iExit;
}

void CPlayerMovement::LateUpdate_Component()
{
    CMovement::LateUpdate_Component();
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
    _vec3 vLook = m_pTransform->Get_Info_Value(INFO_LOOK);
    D3DXVec3Normalize(&vLook, &vLook);
    _vec3 vRight = m_pTransform->Get_Info_Value(INFO_RIGHT);
    D3DXVec3Normalize(&vRight, &vRight);
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
