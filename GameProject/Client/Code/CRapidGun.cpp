#include "pch.h"
#include "CRapidGun.h"
#include "CImGuiTool.h"
#include "CProtoMgr.h"
#include "CDefaultBullet.h"
#include "CSoundMgr.h"
#include "CManagement.h"
#include "CRoomLayer.h"
#include "CRenderer.h"
#include "CStage.h"
#include "CMonster.h"
#include "CClientCameraMgr.h"
#include "CCamera.h"
#include "CWeaponSystem.h"
#include "CRapidGunUltimateMarker.h"
#include "CRapidGunUltimateScope.h"
#include "CRapidGunUltimateTimer.h"
#include "CLayerContext.h"

CRapidGun::CRapidGun(LPDIRECT3DDEVICE9 pGraphicDev)
    : CWeapon(pGraphicDev)
{
}

CRapidGun::~CRapidGun()
{
}

HRESULT CRapidGun::Ready_GameObject()
{
    if (FAILED(CWeapon::Ready_GameObject()))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_fSpecialAtkInterval = 0.1f;

    m_pMarker = CRapidGunUltimateMarker::Create(m_pGraphicDev);
    if (m_pMarker)
    {
        CLayerContext::GetLayer()->Add_GameObject(L"Effect", m_pMarker);
        m_pMarker->Set_IsActive(false);
    }
    m_pScope = CRapidGunUltimateScope::Create(m_pGraphicDev);
    if (m_pScope)
    {
        CLayerContext::GetLayer()->Add_GameObject(L"Scope", m_pScope);
        m_pScope->Set_IsActive(false);
    }
    m_pTimer = CRapidGunUltimateTimer::Create(m_pGraphicDev);
    if (m_pTimer)
    {
        CLayerContext::GetLayer()->Add_GameObject(L"Timer", m_pTimer);
        m_pTimer->Set_IsActive(false);
    }

    return S_OK;
}

_int CRapidGun::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CWeapon::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    UpdateUltimateAttackStatus(fTimeDelta);

    return iExit;
}

void CRapidGun::UpdateUltimateAttackStatus(_float fTimeDelta)
{
    if (!m_bOnUltimateAttack) return;

    m_fTimeAfterUltimate += fTimeDelta;

    if (m_fTimeAfterUltimate > m_fUltimateTime)
    {
        EndUltimateAttack(EInputState::NONE, EInputState::NONE);
        return;
    }

    m_fLeftEffectStartTime -= fTimeDelta;

    if (!m_bEffectStart && m_fLeftEffectStartTime <= 0.f)
    {
        m_bEffectStart = true;
        m_pMarker->Set_IsActive(true);
        m_pScope->Begin();
        m_pTimer->Set_IsActive(true);
    }

    m_pTimer->SetTimeRatio(1.f - m_fTimeAfterUltimate / m_fUltimateTime);

    if (CStage* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene()))
    {
        const vector<CMonster*>& vecMonster = pStage->GetCurrentRoomLayer()->GetMonsterList();

        /* 1인칭을 가정 */
        CCamera* pCamera = CClientCameraMgr::GetInstance()->Find_Camera(CLIENT_CAMERA_TYPE::PLAYER);
        _matrix matWorld;
        pCamera->GetWorld(&matWorld);
        _vec3 vCamPos, vCamLook, vLookNorm;
        memcpy(&vCamLook, &matWorld.m[2][0], sizeof(_vec3));
        memcpy(&vCamPos, &matWorld.m[3][0], sizeof(_vec3));
        D3DXVec3Normalize(&vLookNorm, &vCamLook);

        m_pUltTarget = nullptr;
        float fAngleMin = m_fAngleLimit;
        for (auto p : vecMonster)
        {
            if (p->Is_Dead()) continue;

            /* 시야각이 너무 멀면 패스 */
            CTransform* pTransform = dynamic_cast<CTransform*>(p->Get_Component(ID_DYNAMIC, L"Com_Transform"));
            _vec3 vDisplacement = pTransform->Get_Info_Value(INFO_POS) - vCamPos;
            _vec3 vDirNorm;
            D3DXVec3Normalize(&vDirNorm, &vDisplacement);
            _vec3 vCross;
            D3DXVec3Cross(&vCross, &vDirNorm, &vCamLook);

            float fCos = D3DXVec3Dot(&vLookNorm, &vDirNorm);
            float fSin = D3DXVec3Length(&vCross);
            float fAngle = atan2f(fSin, fCos);

            if (fAngle > m_fAngleLimit) continue;

            /* 가장 시선 방향과 가까운 적을 타겟으로 지정 */
            if (fAngle < fAngleMin)
            {
                fAngleMin = min(fAngleMin, fAngle);
                m_pUltTarget = p;
            }
        }

        if (m_pUltTarget)
        {
            m_pMarker->UpdateTarget(m_pUltTarget);
        }
    }
}

void CRapidGun::LateUpdate_GameObject(_float fTimeDelta)
{
    CWeapon::LateUpdate_GameObject(fTimeDelta);
}

void CRapidGun::Render_GameObject()
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

    // RenderEditorPanel();
}

void CRapidGun::RenderEditorPanel()
{
    bool bTransformUpdated = false;

    ImGui::Begin("Gun");

    ImGui::SeparatorText("Transform");
    bTransformUpdated |= ImGui::DragFloat3("Scale", &m_tLocalFView.vPosition.x, 0.01f, 0.001f, 100.f);
    bTransformUpdated |= ImGui::DragFloat3("Position", &m_tLocalFView.vPosition.x, 0.01f);
    bTransformUpdated |= ImGui::DragFloat3("Rotation", &m_tLocalFView.vPosition.x, 0.5f, -360.f, 360.f);

    ImGui::SeparatorText("Animation");
    ImGui::DragFloat("Move Cycle", &m_fMoveAnimationFrequency, 0.01f, 0.05f, 5.f, "%.2f s");
    ImGui::DragFloat("Horizontal Move", &m_fHorizontalMove, 0.001f, 0.f, 1.f);
    ImGui::DragFloat("Quadratic A", &m_fQuadraticA, 0.001f, 0.f, 1.f);
    ImGui::DragFloat("Max Recoil Angle", &m_fMaxRecoilAngle, 0.5f, -90.f, 0.f);
    ImGui::DragFloat("Recoil Damping", &m_fRecoilDamping, 0.05f, 0.f, 20.f);

    ImGui::End();

    if (bTransformUpdated)
    {
        UpdateLocalTransform(m_tLocalFView);
    }
}

TWeaponOutput CRapidGun::SpecialAttack(EInputState ePri, EInputState eSec)
{
    switch (ePri)
    {
    case EInputState::Held:
    {
        ShotSingleBullet();
        m_bIsCoolTime = true;
        m_fCoolTimeLeft = m_fSpecialAtkInterval;
        return { true, EWeaponAnimEvent::GUN_SHOT };
        break;
    }
    default:
        break;
    }

    return { false, EWeaponAnimEvent::NONE };
}

TWeaponOutput CRapidGun::StartUltimateAttack(EInputState ePri, EInputState eSec)
{
    m_bOnUltimateAttack = true;
    m_fTimeAfterUltimate = 0.f;
    m_pSystem->SetUltimateAttackOnGoing(true);
    CSoundMgr::GetInstance()->PlaySFX(L"Ult_Default.mp3");

    m_bEffectStart = false;
    m_fLeftEffectStartTime = m_fLazyEffectStartTime;

    return { true, EWeaponAnimEvent::ULT_RAPIDGUN };
}

TWeaponOutput CRapidGun::UpdateUltimateAttack(EInputState ePri, EInputState eSec)
{
    switch (ePri)
    {
    case EInputState::Held:
    {
        if (m_pUltTarget)
        {
            CTransform* pTransform = dynamic_cast<CTransform*>(m_pUltTarget->Get_Component(ID_DYNAMIC, L"Com_Transform"));

            /* 타겟에게 총알을 발사 */
            ShotSingleBullet(pTransform->Get_Info_Value(INFO_POS));
            m_bIsCoolTime = true;
            m_fCoolTimeLeft = m_fUltimateAttackInterval;
            return { true, EWeaponAnimEvent::NONE };
        }
        else
        {
            ShotSingleBullet();
            m_bIsCoolTime = true;
            m_fCoolTimeLeft = m_fUltimateAttackInterval;
            return { true, EWeaponAnimEvent::NONE };
        }
        break;
    }
    }

    return { false, EWeaponAnimEvent::NONE };
}

TWeaponOutput CRapidGun::EndUltimateAttack(EInputState ePri, EInputState eSec)
{
    m_bOnUltimateAttack = false;
    m_fTimeAfterUltimate = 0.f;
    m_pSystem->SetUltimateAttackOnGoing(false);

    m_pMarker->Set_IsActive(false);
    m_pScope->End();
    m_pTimer->Set_IsActive(false);

    return { false, EWeaponAnimEvent::NONE };
}

HRESULT CRapidGun::Add_Component()
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

    return S_OK;
}

CRapidGun* CRapidGun::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CRapidGun* pGun = new CRapidGun(pGraphicDev);

    if (FAILED(pGun->Ready_GameObject()))
    {
        Safe_Release(pGun);
        MSG_BOX("CRapidGun Create Failed");
        return nullptr;
    }

    return pGun;
}

void CRapidGun::Free()
{
    CWeapon::Free();
}
