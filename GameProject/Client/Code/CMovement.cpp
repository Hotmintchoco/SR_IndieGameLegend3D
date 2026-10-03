#include "pch.h"
#include "CMovement.h"
#include "CGameObject.h"
#include "CTransform.h"

CMovement::CMovement(LPDIRECT3DDEVICE9 pGraphicDev)
    : CComponent(pGraphicDev)
{
}

CMovement::~CMovement()
{
}

_int CMovement::Update_Component(_float fTimeDelta)
{
    PerformMovement(fTimeDelta);
    ClearFrameVariables();

    return S_OK;
}

void CMovement::LateUpdate_Component()
{
}

void CMovement::AttachTransform(CTransform* pTransform)
{
    if (!pTransform)
    {
        assert(0);
        return;
    }
    m_pTransform = pTransform;
}

void CMovement::PerformMovement(float fTimeDelta)
{
    /* 임펄스 */
    m_vVelocity += m_vImpulse;

    /* 고정 속도 */
    if (m_bPendingLaunch)
    {
        const TLaunchRequest& r = m_tLaunchRequest;
        _vec3 vH{r.vVelocity.x, 0.f, r.vVelocity.z};

        if (r.bOverrideH)
        {
            float fH = D3DXVec3Length(&vH);
            if (fH < 1e-6)
            {
                m_vVelocity.x = 0.f;
                m_vVelocity.z = 0.f;
            }
            else
            {
                _vec3 vDirH = vH / fH;
                float fDot = D3DXVec3Dot(&vDirH, &m_vVelocity);
                float fTarget = fH + r.fAlpha * fDot;
                m_vVelocity.x = vDirH.x * fTarget;
                m_vVelocity.z = vDirH.z * fTarget;
            }
        }
        else
        {
            m_vVelocity.x += r.vVelocity.x;
            m_vVelocity.z += r.vVelocity.z;
        }

        if (r.bOverrideV)
        {
            m_vVelocity.y = r.vVelocity.y;
        }
        else
        {
            m_vVelocity.y += r.vVelocity.y;
        }

        m_bOnGround = false;
        m_bPendingLaunch = false;
    }

    /* 입력 커맨드 */
    m_vVelocity += m_vAccCommand * m_fInputAcc * fTimeDelta;

    /* 중력 및 클리핑 */
    TryExertGravity(fTimeDelta);
    ExertFriction(fTimeDelta);
    ClampVelocity();

    _vec3 vPos = m_pTransform->Get_Info_Value(INFO_POS);
    vPos += m_vVelocity * fTimeDelta;
    /* TODO 실제 지형과 상호작용하도록 */
    if (!m_bOnGround && vPos.y <= 0.f)
    {
        vPos.y = 0;
        m_vVelocity.y = 0;
        m_bOnGround = true;
    }
    m_pTransform->Set_Pos(vPos);
}

void CMovement::TryExertGravity(const float fTimeDelta)
{
    if (m_bOnGround) return;
    
    m_vVelocity.y -= s_fGravity * fTimeDelta;
}

void CMovement::ExertFriction(float fTimeDelta)
{
    float fFriction = (m_bOnGround) ? m_fGroundFriction : m_fAirFriction ;
    
    _vec3 vH = _vec3{ m_vVelocity.x, 0.f, m_vVelocity.z };
    float fSpeedBefore = D3DXVec3Length(&vH);
    if (fSpeedBefore < 1e-6)
    {
        m_vVelocity.x = 0.f;
        m_vVelocity.z = 0.f;
        return;
    }

    float fSpeedAfter = std::max(0.f, fSpeedBefore - fFriction * fTimeDelta );
    vH *= fSpeedAfter / fSpeedBefore;
    
    m_vVelocity.x = vH.x;
    m_vVelocity.z = vH.z;
}

void CMovement::AddImpulse(const _vec3& vDir, float fMagnitude)
{
    _vec3 vDirNorm;
    D3DXVec3Normalize(&vDirNorm, &vDir);
    m_vImpulse += vDirNorm * fMagnitude;
}

void CMovement::ClampVelocity()
{
    float fLength = D3DXVec3Length(&m_vVelocity);
    if (fLength > ((m_bOnGround ? GetCurMaxGroundSpeed() : m_fMaxAirSpeed)))
    {
        _vec3 vVelNorm;
        D3DXVec3Normalize(&vVelNorm, &m_vVelocity);
        m_vVelocity = vVelNorm * (m_bOnGround ? GetCurMaxGroundSpeed() : m_fMaxAirSpeed);
    }
}

void CMovement::ClearFrameVariables()
{
    m_vAccCommand = _vec3{ 0.f, 0.f, 0.f };
    m_vImpulse = _vec3{ 0.f, 0.f, 0.f };
}

void CMovement::Launch(const TLaunchRequest& tReq)
{
    m_tLaunchRequest = tReq;
    m_bPendingLaunch = true;
}

CMovement* CMovement::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    return new CMovement(pGraphicDev);
}

void CMovement::Free()
{
    CComponent::Free();
}
