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

    return S_OK;
}

_int CLiminalGun::Update_GameObject(_float fTimeDelta)
{
    fTimeDelta = CTimerMgr::GetInstance()->GetGroupTimeDelta(TG_PLAYER);

    _int iExit = CWeapon::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    if (m_pHolingObject)
    {
        CalculateView(m_pHolingObject);
    }

    UpdateUltimateAttackState(fTimeDelta);

    return iExit;
}

void CLiminalGun::LateUpdate_GameObject(_float fTimeDelta)
{
    fTimeDelta = CTimerMgr::GetInstance()->GetGroupTimeDelta(TG_PLAYER);

    CWeapon::LateUpdate_GameObject(fTimeDelta);
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
    CTimerMgr::GetInstance()->SetGroupTimeScale(TG_PLAYER, 0.5f);
    m_bOnUltimateAttack = true;
    m_fTimeAfterUltimate = 0.f;
    m_pSystem->SetUltimateAttackOnGoing(true);

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
                if (p->Is_Dead()) continue;

                CHitScan* pHitScan = CHitScan::Create(m_pGraphicDev, m_vBulletFrom, p);
                if (pHitScan) CManagement::GetInstance()->GetCurrentScene()->Add_GameObject(L"HitScan", pHitScan);
            }
        }

        EndUltimateAttack(EInputState::NONE, EInputState::NONE);
        return { true, EWeaponAnimEvent::ULT_LIMINALGUN_END };
        break;
    }
    }

    return { false, EWeaponAnimEvent::ULT_LIMINALGUN_LOOP };
}

TWeaponOutput CLiminalGun::EndUltimateAttack(EInputState ePri, EInputState eSec)
{
    CTimerMgr::GetInstance()->SetGlobalTimeScale(1.f);
    CTimerMgr::GetInstance()->ClearGroupTimeScale(TG_PLAYER);
    m_fDmgAccumulated = 0.f;
    m_bOnUltimateAttack = false;
    m_fTimeAfterUltimate = 0.f;
    m_pSystem->SetUltimateAttackOnGoing(false);

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
    m_matCapture = (*pTransform->Get_World()) * matCamInv;
    memcpy(&m_vCaptureDisplacement, &m_matCapture.m[3][0], sizeof(_vec3));
    m_fCaptureDist = D3DXVec3Length(&m_vCaptureDisplacement);
    _vec3 vLook = pTransform->Get_Info_Value(INFO_LOOK);
    m_fCaptureScale = D3DXVec3Length(&vLook); /* 균등 스케일 가정 */
}

void CLiminalGun::CalculateView(CLiminalObject* pObject)
{
    CCamera* pCamera = CClientCameraMgr::GetInstance()->Find_Camera(CLIENT_CAMERA_TYPE::PLAYER);
    if (!pCamera) return;
    _vec3 vCamLook, vCamPos;
    _matrix matCamWorld;
    pCamera->GetWorld(&matCamWorld);
    memcpy(&vCamPos, &matCamWorld.m[3][0], sizeof(_vec3));

    /* 현재 카메라 행렬에 대한 물체 위치 재계산 */
    _vec3 vDisplacement;
    D3DXVec3TransformNormal(&vDisplacement, &m_vCaptureDisplacement, &matCamWorld);
    _vec3 vDisplacementNorm;
    D3DXVec3Normalize(&vDisplacementNorm, &vDisplacement);

    /* 현재 카메라 행렬에 대한 물체 회전 재계산 (회전만 씀) */
    _matrix matCurRot = m_matCapture * matCamWorld;
    pObject->GetTransform()->Set_World(&matCurRot);    
    pObject->GetTransform()->WorldMatrixDecompose();

    /* 꼭짓점에 대해 ray를 쏘아서 적절한 스케일 값 구하기 */
    THitInfo tHit{};
    CRayCaster* pRayCaster = static_cast<CRayCaster*>(CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"RayCaster"));

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
                _vec3 vDir = vVtxWorld - vCamPos;
                D3DXVec3Normalize(&vDir, &vDir);

                CStage* pStage = static_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());
                const vector<IRayTestable*> mapObject = pStage->GetCurrentRoomLayer()->GetRayTestableList();
                CRayCaster* pRayCaster = static_cast<CRayCaster*>(CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"RayCaster"));

                for (auto& pObj : mapObject)
                {
                    if (pObj == m_pHolingObject) continue; /* 본인은 통과 */
                         
                    const vector<pair<CVIBuffer*, CTransform*>>& vecInfoDst = pObj->GetRayTestTargetInfo();
                    for (auto& [pBufferDst, pTransformDst] : vecInfoDst)
                    {
                        pRayCaster->RayTest(tHit, vCamPos, vDir, pBufferDst, pTransformDst->Get_World());
                    }
                }
            }
        }
    }

    if (tHit.bHit)
    {
        float fDist = tHit.fDist;

        float fCurScale = pObject->GetTransform()->Get_Scale().x; /* 균등이니깐 그냥 x만 */
        float fMargin = fCurScale * sqrtf(3.f) / 2.f / 5.f; /* 명확한 기준을 잡기 어려워서 휴리스틱하게 */
        // float fMargin = 0;

        pObject->GetTransform()->Set_Pos(vCamPos + vDisplacementNorm * (fDist - fMargin));
        float fNewScale = m_fCaptureScale * (fDist - fMargin) / m_fCaptureDist;
        pObject->GetTransform()->Set_Scale(fNewScale, fNewScale, fNewScale);
    }
    else
    {
        /* 충돌이 없다면 (아마 하늘을 바라보면) 캡쳐 시점으로 유지 */
        pObject->GetTransform()->Set_Pos(vCamPos + vDisplacement);
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
    if (!m_bOnUltimateAttack) return;

    m_fTimeAfterUltimate += fTimeDelta;
    m_fDmgAccumulated = m_fTimeAfterUltimate * m_fDmgPerSecond;

    if (CStage* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene()))
    {
        const vector<CMonster*>& vecMonster = pStage->GetCurrentRoomLayer()->GetMonsterList();

        for (auto p : vecMonster)
        {
            if (p->Is_Dead()) continue;

            CTransform* pTransform = dynamic_cast<CTransform*>(p->Get_Component(ID_DYNAMIC, L"Com_Transform"));

            int iHp = 10; // TODO int iHp = p->GetHp();
            float fRatio = clamp(1.f - m_fDmgAccumulated / (float)iHp, 0.f, 1.f);

            /* 디버깅 */
            DWORD dwColor = (fRatio == 0.f) ? D3DCOLOR_ARGB(255, 255, 0, 0) : D3DCOLOR_ARGB(255, 0, 255, 0);
            CRenderer::GetInstance()->Add_DebugWorldRect(m_pGraphicDev, pTransform->Get_Info_Value(INFO_POS), fRatio * m_fMaxRadius + (1.f - fRatio) * m_fMinRadius, dwColor);
        }
    }
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
