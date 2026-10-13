#include "pch.h"
#include "CFlatPlayerMovement.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "CManagement.h"
#include "CStage.h"
#include "IVerticalTerrain.h"
#include "COrthoRoomLayer.h"

CFlatPlayerMovement::CFlatPlayerMovement(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMovement(pGraphicDev)
{
    m_fMaxGroundSpeed = 2.5f;
    m_fMaxAirSpeedH = 2.5f;
}

CFlatPlayerMovement::~CFlatPlayerMovement()
{
}

_int CFlatPlayerMovement::Update_Component(_float fTimeDelta)
{
    CMovement::Update_Component(fTimeDelta);

    return S_OK;
}

void CFlatPlayerMovement::LateUpdate_Component()
{
    CMovement::LateUpdate_Component();
}

void CFlatPlayerMovement::Walk(const float fCommand)
{
    _vec3 vDir;
    if (fCommand == 0.f)
    {
        vDir = { 0.f, 0.f, 0.f };
    }
    else
    {
        vDir = { (fCommand > 0.f) ? 1.f : -1.f, 0.f, 0.f };
    }

    /* 우측 +, 좌측 - */
    m_vAccCommand = vDir * ((!m_bOnGround) ? m_fAirInputCoef : 1.f);
}

void CFlatPlayerMovement::Jump()
{
    if (!m_bOnGround) return;

    m_bOnGround = false;
    AddImpulse(_vec3{ 0.f, 0.f, 1.f }, m_fJumpSpeed);
}

void CFlatPlayerMovement::PerformMovement(float fTimeDelta)
{
    /* 임펄스 */
    m_vVelocity += m_vImpulse;

    /* 입력 커맨드만 사용 */
    m_vVelocity += m_vAccCommand * m_fInputAcc * fTimeDelta;

    /* 중력 및 클리핑 */
    TryExertGravity(fTimeDelta);
    ExertFriction(fTimeDelta);
    ClampVelocity();

    _vec3 vPrevPos = m_pTransform->Get_Info_Value(INFO_POS);
    _vec3 vDesiredPos = vPrevPos + m_vVelocity * fTimeDelta;

    CStage* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());
    if (pStage)
    {
        const vector<IVerticalTerrain*> vecTerrain = dynamic_cast<COrthoRoomLayer*>(pStage->GetCurrentRoomLayer())->GetVerticalTerrainList();
        TerrainResolver(vecTerrain, vDesiredPos);
    }
    else
    {
        if (!m_bOnGround && vDesiredPos.z <= 0.f)
        {
            vDesiredPos.z = 0;
            m_vVelocity.z = 0;
            m_bOnGround = true;
        }
    }

    m_pTransform->Set_Pos(vDesiredPos);
}

void CFlatPlayerMovement::TryExertGravity(const float fTimeDelta)
{
    if (m_bOnGround) return;

    m_vVelocity.z -= m_fGravity * fTimeDelta;
}

void CFlatPlayerMovement::ExertFriction(float fTimeDelta)
{
    float fFriction = (m_bOnGround) ? m_fGroundFriction : m_fAirFriction;

    _vec3 vH = _vec3{ m_vVelocity.x, 0.f, 0.f };
    float fSpeedBefore = D3DXVec3Length(&vH);
    if (fSpeedBefore < 1e-6)
    {
        m_vVelocity.x = 0.f;
        return;
    }

    float fSpeedAfter = std::max(0.f, fSpeedBefore - fFriction * fTimeDelta);
    vH *= fSpeedAfter / fSpeedBefore;

    m_vVelocity.x = vH.x;
}

void CFlatPlayerMovement::ClampVelocity()
{
    /* 수평 성분 */
    _vec3 vVelH{ m_vVelocity.x, 0.f, 0.f };
    float fVelSpeedH = D3DXVec3Length(&vVelH);
    float fVelLimitH = (m_bOnGround ? GetCurMaxGroundSpeed() : m_fMaxAirSpeedH);
    if (fVelSpeedH > fVelLimitH)
    {
        _vec3 vVelNorm;
        D3DXVec3Normalize(&vVelNorm, &vVelH);
        m_vVelocity.x = vVelNorm.x * fVelLimitH;
    }

    /* 수직 성분 */
    float fVelSpeedV = fabs(m_vVelocity.z);
    float fVelLimitV = (m_bOnGround ? FLT_MAX : m_fMaxAirSpeedV);
    if (fVelSpeedV > fVelLimitV)
    {
        m_vVelocity.z = std::max(m_vVelocity.z, -m_fMaxAirSpeedV);
    }
}


void CFlatPlayerMovement::TerrainResolver(const vector<IVerticalTerrain*>& vecTerrain, _vec3& vDesired)
{
    float fHeight = -FLT_MAX;
    _vec3 vRayStart = vDesired + _vec3{ 0.f, 0.f, m_fSkin };
    for (auto pTerrain : vecTerrain)
    {
        fHeight = max(fHeight, pTerrain->SampleTerrainHeight(vRayStart));
    }

    if (fHeight + m_fSkin >= vDesired.z && m_vVelocity.z <= 0.f)
    {
        vDesired.z = fHeight;
        m_vVelocity.z = 0.f;
        m_bOnGround = true;
    }
    else
    {
        m_bOnGround = false;
    }
}

CFlatPlayerMovement* CFlatPlayerMovement::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    return new CFlatPlayerMovement(pGraphicDev);
}

void CFlatPlayerMovement::Free()
{
    CMovement::Free();
}
