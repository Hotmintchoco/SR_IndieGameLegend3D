#include "pch.h"
#include "CFlatPlayerMovement.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "CManagement.h"
#include "CStage.h"
#include "IVerticalTerrain.h"
#include "COrthoRoomLayer.h"
#include "CFlatPlayer.h"
#include "CSoundMgr.h"

CFlatPlayerMovement::CFlatPlayerMovement(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMovement(pGraphicDev)
{
    m_fMaxGroundSpeed = 2.5f;
    m_fMaxAirSpeedH = 2.5f;
    m_fAirFriction = 0.3f;
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
    CSoundMgr::GetInstance()->PlaySFX(L"Ortho_Jump.mp3");
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
    COrthoRoomLayer* pLayer = pStage ? dynamic_cast<COrthoRoomLayer*>(pStage->GetCurrentRoomLayer()) : nullptr;
    if (pLayer)
    {
        const vector<IVerticalTerrain*>& vecTerrain = pLayer->GetVerticalTerrainList();
        TerrainResolver(vecTerrain, vPrevPos, vDesiredPos);
        RoomBoundsResolver(pLayer, vDesiredPos);
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


void CFlatPlayerMovement::TerrainResolver(const vector<IVerticalTerrain*>& vecTerrain, const _vec3& vPrev, _vec3& vDesired)
{
    /* 피벗 : 발밑 가운데 → 플레이어 영역 x : [x - fHalfW, x + fHalfW], z : [z, z + fHeight] */
    const _vec2 vSize = GetFlatSize();
    const float fHalfW = vSize.x * 0.5f;
    const float fHeight = vSize.y;
    const float fEps = 1e-4f;

    struct TBox { _vec2 vMin; _vec2 vMax; };
    vector<TBox> vecBox;
    vecBox.reserve(vecTerrain.size());
    for (auto pTerrain : vecTerrain)
    {
        TBox tBox;
        if (pTerrain->GetFlatSolidBounds(tBox.vMin, tBox.vMax))
            vecBox.push_back(tBox);
    }

    auto OverlapX = [&](const TBox& t, float x) { return x + fHalfW > t.vMin.x + fEps && x - fHalfW < t.vMax.x - fEps; };
    auto OverlapZ = [&](const TBox& t, float z) { return z + fHeight > t.vMin.y + fEps && z < t.vMax.y - fEps; };

    /* 1. 옆면 : 이전 높이에서 x만 이동해보고 겹치면 이동 방향 반대편 면에 붙임 */
    const float fDX = vDesired.x - vPrev.x;
    if (fDX != 0.f)
    {
        for (const TBox& t : vecBox)
        {
            if (!OverlapZ(t, vPrev.z) || !OverlapX(t, vDesired.x))
                continue;

            if (fDX > 0.f) vDesired.x = min(vDesired.x, t.vMin.x - fHalfW);
            else           vDesired.x = max(vDesired.x, t.vMax.x + fHalfW);
            m_vVelocity.x = 0.f;
        }
    }

    /* 2. 위아래 : 확정된 x에서 z 이동 */
    const float fDZ = vDesired.z - vPrev.z;
    bool bLanded = false;
    float fLandZ = -FLT_MAX;

    for (const TBox& t : vecBox)
    {
        if (!OverlapX(t, vDesired.x))
            continue;

        if (fDZ <= 0.f)
        {
            /* 착지 : 이전엔 윗면 위에 있었고 이번에 윗면(+스킨) 아래로 내려옴 */
            if (vPrev.z >= t.vMax.y - fEps && vDesired.z <= t.vMax.y + m_fSkin)
            {
                bLanded = true;
                fLandZ = max(fLandZ, t.vMax.y);
            }
        }
        else
        {
            /* 머리 : 이전엔 밑면 아래였고 이번에 밑면을 넘음 */
            if (vPrev.z + fHeight <= t.vMin.y + fEps && vDesired.z + fHeight > t.vMin.y)
            {
                vDesired.z = t.vMin.y - fHeight;
                m_vVelocity.z = 0.f;
            }
        }
    }

    if (bLanded)
    {
        vDesired.z = fLandZ;
        m_vVelocity.z = 0.f;
        m_bOnGround = true;
    }
    else
    {
        m_bOnGround = false;
    }
}

_vec2 CFlatPlayerMovement::GetFlatSize()
{
    if (CFlatPlayer* pFlatPlayer = dynamic_cast<CFlatPlayer*>(Get_Owner()))
        return pFlatPlayer->GetFlatSize();

    return _vec2{ 1.f, 1.f };
}

void CFlatPlayerMovement::RoomBoundsResolver(COrthoRoomLayer* pLayer, _vec3& vDesired)
{
    /* 크기는 2D 플레이어가 알려줌 (피벗 : 발밑 가운데) */
    const _vec2 vSize = GetFlatSize();

    _vec2 vMin, vMax;
    pLayer->GetFlatBounds(vMin, vMax);

    const float fHalfW = vSize.x * 0.5f;

    /* 좌우 벽 */
    if (vDesired.x < vMin.x + fHalfW)
    {
        vDesired.x = vMin.x + fHalfW;
        m_vVelocity.x = max(m_vVelocity.x, 0.f);
    }
    else if (vDesired.x > vMax.x - fHalfW)
    {
        vDesired.x = vMax.x - fHalfW;
        m_vVelocity.x = min(m_vVelocity.x, 0.f);
    }

    /* 천장 */
    if (vDesired.z > vMax.y - vSize.y)
    {
        vDesired.z = vMax.y - vSize.y;
        m_vVelocity.z = min(m_vVelocity.z, 0.f);
    }

    /* 바닥 : 방 아래 끝은 땅처럼 착지 */
    if (vDesired.z < vMin.y)
    {
        vDesired.z = vMin.y;
        m_vVelocity.z = 0.f;
        m_bOnGround = true;
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
