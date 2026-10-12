#include "pch.h"
#include "CLiminalGun.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CLiminalObject.h"
#include "CRayCaster.h"
#include "CManagement.h"
#include "IRayTestable.h"
#include "CStage.h"
#include "CRoomLayer.h"
#include "CClientCameraMgr.h"
#include "CCamera.h"
#include "CWeaponSystem.h"
#include "CTimerMgr.h"
#include "CMonster.h"
#include "CHitScan.h"
#include "CSoundMgr.h"
#include "CLiminalGunUltimateEffect.h"
#include "CLayerContext.h"
#include "CLiminalPortal.h"
#include "CShaderEffectMgr.h"
#include "CGrayscaleEffect.h"

CLiminalGun::CLiminalGun(LPDIRECT3DDEVICE9 pGraphicDev)
    : CWeapon(pGraphicDev)
{
    D3DXMatrixIdentity(&m_matCapture);
}

CLiminalGun::~CLiminalGun()
{
}

HRESULT CLiminalGun::Ready_GameObject()
{
    if (FAILED(CWeapon::Ready_GameObject()))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_fGaugeConsumePerSpecialAtk = 0.f;

    /* GameLogic Layer */
    CLayer* pLayer = CLayerContext::GetLayer();
    if (!pLayer)
    {
        if (CStage* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene()))
        {
            pLayer = pStage->Get_Layer(L"GameLogic_Layer");
        }
    }

    m_pEffect = CLiminalGunUltimateEffect::Create(m_pGraphicDev);
    if (m_pEffect)
    {
        pLayer->Add_GameObject(L"Effect", m_pEffect);
        m_pEffect->Set_IsActive(false);
    }

    m_pShaderEffect = dynamic_cast<CGrayscaleEffect*>(CShaderEffectMgr::GetInstance()->Get_PostEffect(POST_EFFECT::GRAYSCALE));
    if (!m_pShaderEffect)
        return E_FAIL;

    return S_OK;
}

_int CLiminalGun::Update_GameObject(_float fTimeDelta)
{
    fTimeDelta = CTimerMgr::GetInstance()->GetGroupTimeDelta(CTG_PLAYER);

    _int iExit = CWeapon::Update_GameObject(fTimeDelta);

    if (m_bOverlay)
    {
        CRenderer::GetInstance()->Add_RenderGroup(RENDER_OVERLAY, this);
    }
    else
    {
        CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
    }

    UpdateUltimateAttackState(fTimeDelta);

    UpdateHitScanState(fTimeDelta);

    if (m_bOnEffectTransition)
    {
        m_fTimeAfterEffect += fTimeDelta;
        float fRatio = clamp(m_fTimeAfterEffect / m_fTotalEffectTransitionTime, 0.f, 1.f);
        float s = fRatio * m_fEndStrength + (1.f - fRatio) * m_fStartStrength;
        m_pShaderEffect->Set_Strength(s);

        if (m_fTimeAfterEffect >= m_fTotalEffectTransitionTime)
        {
            m_bOnEffectTransition = false;
            if (m_bOnUltimateAttack) /* 궁극기 진입 시*/
            {
                CShaderEffectMgr::GetInstance()->Set_PostEffect(POST_EFFECT::GRAYSCALE);
            }
            else /* 끝나고 되돌아갈때 */
            {
                CShaderEffectMgr::GetInstance()->Set_PostEffect(m_eOldEffectType);
            }
        }
    }

    return iExit;
}

void CLiminalGun::LateUpdate_GameObject(_float fTimeDelta)
{
    fTimeDelta = CTimerMgr::GetInstance()->GetGroupTimeDelta(CTG_PLAYER);

    CWeapon::LateUpdate_GameObject(fTimeDelta);


    if (m_pHolingObject)
    {
        CalculateView(m_pHolingObject);
    }
}

void CLiminalGun::Render_GameObject()
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
}

TWeaponOutput CLiminalGun::SpecialAttack(EInputState ePri, EInputState eSec)
{
    switch (ePri)
    {
    case EInputState::Pressed:
    {
        RayCastToLiminalObject();
        return { false, EWeaponAnimEvent::NONE };
        break;
    }
    case EInputState::Held:
    {
        break;
    }
    case EInputState::Released:
    {
        if (m_pHolingObject)
        {
            m_pHolingObject->SetGrabbed(false);
            m_pHolingObject = nullptr;
            D3DXMatrixIdentity(&m_matCapture);
            m_vCaptureDisplacement = _vec3{ 0.f, 0.f, 0.f };
        }
        break;
    }
    default:
        break;
    }

    switch (eSec)
    {
    case EInputState::Pressed:
        if (m_pHolingObject)
        {
            AdjustRotation(m_pHolingObject);
        }
        break;
    default:
        break;
    }

    return { false, EWeaponAnimEvent::NONE };
}

TWeaponOutput CLiminalGun::StartUltimateAttack(EInputState ePri, EInputState eSec)
{
    CTimerMgr::GetInstance()->SetGlobalTimeScale(0.1f);
    CTimerMgr::GetInstance()->SetGroupTimeScale(CTG_PLAYER, 0.3f);
    m_bOnUltimateAttack = true;
    m_fTimeAfterUltimate = 0.f;
    m_pSystem->SetUltimateAttackOnGoing(true);
    CSoundMgr::GetInstance()->PlaySFX(L"Ult_Liminal.wav");

    m_pEffect->Set_IsActive(true);

    m_bOnEffectTransition = true;
    m_fTotalEffectTransitionTime = 1.f;
    m_fTimeAfterEffect = 0.f;
    m_fStartStrength = 0.f;
    m_fEndStrength = 1.f;
    m_eOldEffectType = CShaderEffectMgr::GetInstance()->Get_PostEffectType();
    CShaderEffectMgr::GetInstance()->Set_PostEffect(POST_EFFECT::GRAYSCALE);


    return { true, EWeaponAnimEvent::ULT_LIMINALGUN_START };
}

TWeaponOutput CLiminalGun::UpdateUltimateAttack(EInputState ePri, EInputState eSec)
{
    switch (ePri)
    {
    case EInputState::Pressed:
    {
        if (CStage* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene()))
        {
            const vector<CMonster*>& vecMonster = pStage->GetCurrentRoomLayer()->GetMonsterList();

            for (auto p : vecMonster)
            {
                float fYaw = GetYawFromCameraToTarget(p);
                m_pqCapture.push({p, fYaw});
            }
            m_bOnHitScan = true;
        }
        return { true, EWeaponAnimEvent::ULT_LIMINALGUN_END };
        break;
    }
    }

    return { false, EWeaponAnimEvent::ULT_LIMINALGUN_LOOP };
}

TWeaponOutput CLiminalGun::EndUltimateAttack(EInputState ePri, EInputState eSec)
{
    CTimerMgr::GetInstance()->SetGlobalTimeScale(1.f);
    CTimerMgr::GetInstance()->ClearAllGroupTimeScale();
    m_fDmgAccumulated = 0.f;
    m_bOnUltimateAttack = false;
    m_fTimeAfterUltimate = 0.f;
    m_pSystem->SetUltimateAttackOnGoing(false);
    m_bOnHitScan = false;

    m_bOnEffectTransition = true;
    m_fTotalEffectTransitionTime = 0.3f;
    m_fTimeAfterEffect = 0.f;
    m_fStartStrength = 1.f;
    m_fEndStrength = 0.f;

    return { false, EWeaponAnimEvent::NONE };
}

void CLiminalGun::RayCastToLiminalObject()
{
    CStage* pStage = static_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());

    CCamera* pCamera = CClientCameraMgr::GetInstance()->Find_Camera(CLIENT_CAMERA_TYPE::PLAYER);
    if (!pCamera) return;
    _vec3 vCamLook, vCamPos;
    _matrix matCamWorld;
    pCamera->GetWorld(&matCamWorld);
    memcpy(&vCamLook, &matCamWorld.m[2][0], sizeof(_vec3));
    memcpy(&vCamPos, &matCamWorld.m[3][0], sizeof(_vec3));

    const vector<IRayTestable*>& mapObject = pStage->GetCurrentRoomLayer()->GetRayTestableList();
    CRayCaster* pRayCaster = static_cast<CRayCaster*>(CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"RayCaster"));

    THitInfo tHit{};

    for (auto& pObj : mapObject)
    {
        const vector<pair<CVIBuffer*, CTransform*>>& vecInfo = pObj->GetRayTestTargetInfo();
        for (auto& [pBuffer, pTransform] : vecInfo)
        {
            pRayCaster->RayTest(tHit, vCamPos, vCamLook, pBuffer, pTransform->Get_World());
        }
    }

    if (tHit.bHit)
    {
        m_pHolingObject = dynamic_cast<CLiminalObject*>(tHit.pObject);
        if (m_pHolingObject)
        {
            m_pHolingObject->SetGrabbed(true);
            
            CaptureTransform(m_pHolingObject);
        }
    }
}

void CLiminalGun::CaptureTransform(CLiminalObject* pObject)
{
    /* 파지 시점의 변환 캡쳐 */
    CCamera* pCamera = CClientCameraMgr::GetInstance()->Find_Camera(CLIENT_CAMERA_TYPE::PLAYER);
    if (!pCamera) return;
    _vec3 vCamLook, vCamPos;
    _matrix matCamWorld;
    pCamera->GetWorld(&matCamWorld);
    memcpy(&vCamLook, &matCamWorld.m[2][0], sizeof(_vec3));
    memcpy(&vCamPos, &matCamWorld.m[3][0], sizeof(_vec3));

    CTransform* pTransform = pObject->GetTransform();
    _matrix matCamInv;
    D3DXMatrixInverse(&matCamInv, nullptr, &matCamWorld);
    /* From : 물체의 로컬 공간 좌표 , To : 물체의 카메라 로컬 공간 좌표 이니까 */
    /* 물체의 로컬 공간 좌표 * 물체 월드 행렬 * 카메라 월드 역행렬 -> 물체의 카메라 로컬 공간 좌표 */
    m_matCapture = (*pTransform->Get_World()) * matCamInv;
    m_fCaptureDist = D3DXVec3Length(&m_vCaptureDisplacement);
    _vec3 vLook = pTransform->Get_Info_Value(INFO_LOOK);
    m_fCaptureScale = D3DXVec3Length(&vLook); /* 균등 스케일 가정 */
}

void CLiminalGun::CalculateView(CLiminalObject* pObject)
{
    /* 계산 순서 : 바뀐 View에 대해 회전을 다시 계산 후 스케일(위치) 계산 */

    CCamera* pCamera = CClientCameraMgr::GetInstance()->Find_Camera(CLIENT_CAMERA_TYPE::PLAYER);
    if (!pCamera) return;
    _vec3 vCamLook, vCamPos;
    _matrix matCamWorld;
    pCamera->GetWorld(&matCamWorld);
    memcpy(&vCamPos, &matCamWorld.m[3][0], sizeof(_vec3));

    /* 현재 카메라 행렬에 대한 물체 회전 재계산 (회전만 씀) */
    _matrix matCurRot = m_matCapture * matCamWorld;
    pObject->GetTransform()->Set_World(&matCurRot);    
    pObject->GetTransform()->WorldMatrixDecompose();

    /* 꼭짓점에 대해 ray를 쏘아서 적절한 스케일 값 구하기 */
    CStage* pStage = static_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());
    const vector<IRayTestable*>& vecRayTargets = pStage->GetCurrentRoomLayer()->GetRayTestableList();
    CRayCaster* pRayCaster = static_cast<CRayCaster*>(CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"RayCaster"));

    float fMinRatio = FLT_MAX; /* 매 프레임마다 현재 스케일에 얼마만큼 곱해줄지 비율을 계산함 */

    const vector<pair<CVIBuffer*, CTransform*>>& vecInfoSrc = m_pHolingObject->GetRayTestTargetInfo();
    for (auto& [pBufferSrc, pTransformSrc] : vecInfoSrc)
    {
        const vector<TTriInfo>& vVtx = pBufferSrc->GetTri();
        for (auto& v : vVtx)
        {
            for (int i = 0; i < 3; ++i)
            {
                _vec3 vVtxWorld;
                D3DXVec3TransformCoord(&vVtxWorld, &v.vTriPos[i], pTransformSrc->Get_World());
                _vec3 vDir = vVtxWorld - vCamPos; /* 들고 있는 물체의 한점을 향하는 카메라에서 출발하는 레이 */
                float fDist = D3DXVec3Length(&vDir); /* 그 점까지의 거리 */
                if (fDist < 1e-4f) continue; /* 너무 가까운 경우에는 통과 */
                vDir /= fDist; 

                THitInfo tHit{};

                for (auto& pObj : vecRayTargets)
                {
                    if (pObj == m_pHolingObject) continue; /* 본인은 통과 */
                         
                    const vector<pair<CVIBuffer*, CTransform*>>& vecInfoDst = pObj->GetRayTestTargetInfo();
                    for (auto& [pBufferDst, pTransformDst] : vecInfoDst)
                    {
                        pRayCaster->RayTest(tHit, vCamPos, vDir, pBufferDst, pTransformDst->Get_World());
                    }
                }

                if (tHit.fDist != FLT_MAX)
                {
                    /* tHit.fDist / fDist : 점을 연장해서 배경 메쉬에 맞은 순간의 선분 길이와,
                    *  카메라 - 파지 대상의 버텍스 길이의 비율. 이 중 가장 작은 값이 실제 늘려도 벽에 파묻히지 않는 최댓값이 된다
                    */
                    fMinRatio = min(fMinRatio, tHit.fDist / fDist);
                }
            }
        }
    }

    _vec3 vPos = pObject->GetTransform()->Get_Info_Value(INFO_POS);
    if (fMinRatio < FLT_MAX)
    {
        float fMargin = 0.98f;
        float s = fMinRatio * fMargin;

        float fScale = pObject->GetTransform()->Get_Scale().x; /* 균등이니깐 그냥 x만 */
        pObject->GetTransform()->Set_Scale(fScale * s, fScale * s, fScale * s);
        pObject->GetTransform()->Set_Pos(vCamPos + (vPos - vCamPos) * s);
    }
    else
    {
        /* 충돌이 없다면 (아마 하늘을 바라보면) 캡쳐 시점으로 유지 */
        pObject->GetTransform()->Set_Pos(vCamPos + (vPos - vCamPos));
    }
}

void CLiminalGun::AdjustRotation(CLiminalObject* pObject)
{
    _vec3 vRot = pObject->GetTransform()->Get_Rotation();
    pObject->GetTransform()->Set_Rotation_Raw(_vec3{ 0.f, vRot.y, 0.f });

    /* 캡쳐 당시의 트랜스폼도 변경 */
    CaptureTransform(m_pHolingObject);
}

void CLiminalGun::UpdateUltimateAttackState(_float fTimeDelta)
{
    if (!m_bOnUltimateAttack || m_bOnHitScan) return;

    m_fTimeAfterUltimate += fTimeDelta;
    m_fDmgAccumulated = m_fTimeAfterUltimate * m_fDmgPerSecond;

    if (CStage* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene()))
    {
        const vector<CMonster*>& vecMonster = pStage->GetCurrentRoomLayer()->GetMonsterList();

        for (auto p : vecMonster)
        {
            if (p->Is_Dead()) continue;

            CTransform* pTransform = dynamic_cast<CTransform*>(p->Get_Component(ID_DYNAMIC, L"Com_Transform"));

            int iMaxHp = p->Get_MaxHp();
            int iHp = p->Get_Hp();
            float fRatio = clamp(1.f - (iHp - m_fDmgAccumulated) / (float)iMaxHp, 0.f, 1.f);
            
            m_pEffect->AddTargetInfo(TLiminalUltimateTargetInfo{ pTransform->Get_Info_Value(INFO_POS), fRatio });
        }
    }
}

void CLiminalGun::UpdateHitScanState(float fTimeDelta)
{
    if (!m_bOnHitScan) return;

    m_fTimeAfterHitScan += fTimeDelta;

    if (m_fTimeAfterHitScan < m_fSingleHitScanInverval) return;
    
    m_fTimeAfterHitScan -= m_fSingleHitScanInverval;

    if (m_pqCapture.empty())
    {
        EndUltimateAttack(EInputState::NONE, EInputState::NONE);
        return;
    }

    const TLiminalGunHitScanInfo& tInfo = m_pqCapture.top();

    if (!tInfo.pTarget->Is_Dead())
    {
        CHitScan* pHitScan = CHitScan::Create(m_pGraphicDev, m_vBulletFrom, tInfo.pTarget, m_fDmgAccumulated);
        if (pHitScan) CManagement::GetInstance()->GetCurrentScene()->Add_GameObject(L"HitScan", pHitScan);
    }

    m_pqCapture.pop();

    StartShotAnimation();
}

float CLiminalGun::GetYawFromCameraToTarget(CMonster* pMonster)
{
    _vec3 vPos;
    pMonster->Get_Pos(&vPos);

    _vec3 vCamPos, vCamLook;
    CCamera* pCamera = CClientCameraMgr::GetInstance()->Find_Camera(CLIENT_CAMERA_TYPE::PLAYER);
    _matrix matCamWorld;
    pCamera->GetWorld(&matCamWorld);
    memcpy(&vCamPos, &matCamWorld.m[3][0], sizeof(_vec3));
    memcpy(&vCamLook, &matCamWorld.m[2][0], sizeof(_vec3));

    _vec3 vDisplacement = vPos - vCamPos;

    _vec3 vFlat = _vec3{ vDisplacement.x, 0.f, vDisplacement.z };
    if (D3DXVec3LengthSq(&vFlat) < 1e-6f)
        return 0.f;
    D3DXVec3Normalize(&vFlat, &vFlat);
    _vec3 vCamFlat = _vec3{ vCamLook.x, 0.f, vCamLook.z };
    D3DXVec3Normalize(&vCamFlat, &vCamFlat);

    _vec3 vCross;
    D3DXVec3Cross(&vCross, &vCamFlat, &vFlat);

    float fCos = D3DXVec3Dot(&vCamFlat, &vFlat);
    float fSin = vCross.y;
    float fYaw = atan2f(fSin, fCos);

    return fYaw;
}

HRESULT CLiminalGun::Add_Component()
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

CLiminalGun* CLiminalGun::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CLiminalGun* pGun = new CLiminalGun(pGraphicDev);

    if (FAILED(pGun->Ready_GameObject()))
    {
        Safe_Release(pGun);
        MSG_BOX("CLiminalGun Create Failed");
        return nullptr;
    }

    return pGun;
}

void CLiminalGun::Free()
{
    CWeapon::Free();
}
