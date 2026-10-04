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
#include "CCameraMgr.h"

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

    UpdateLocalTransform(m_vScaleLocal, m_vRotationLocal, m_vPositionLocal);

    return S_OK;
}

_int CLiminalGun::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CWeapon::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    if (m_pHolingObject)
    {
        CalculateView(m_pHolingObject);
    }

    return iExit;
}

void CLiminalGun::LateUpdate_GameObject(_float fTimeDelta)
{
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

EWeaponEvent CLiminalGun::SpecialAttack(EInputState ePri, EInputState eSec)
{
    switch (ePri)
    {
    case EInputState::Pressed:
    {
        RayCastToLiminalObject();
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

    return EWeaponEvent::NONE;
}

EWeaponEvent CLiminalGun::UltimateAttack(EInputState ePri, EInputState eSec)
{
    return EWeaponEvent::NONE;
}

void CLiminalGun::RayCastToLiminalObject()
{
    CStage* pStage = static_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());

    /* TODO */
    CCameraObj* pCamera = CCameraMgr::GetInstance()->GetCamera(L"Camera_Player_FPV");
    if (!pCamera) return;
    _vec3 vCamLook, vCamPos;
    _matrix matCamWorld;
    pCamera->GetWorld(&matCamWorld);
    memcpy(&vCamLook, &matCamWorld.m[2][0], sizeof(_vec3));
    memcpy(&vCamPos, &matCamWorld.m[3][0], sizeof(_vec3));
    /* */

    const multimap<wstring, CGameObject*>& mapObject = pStage->GetCurrentRoomLayer()->Get_ObjMap();
    CRayCaster* pRayCaster = static_cast<CRayCaster*>(CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"RayCaster"));

    THitInfo tHit{};

    for (auto& [wstrName, pObject] : mapObject)
    {
        if (IRayTestable* pRayTestable = dynamic_cast<IRayTestable*>(pObject))
        {
            const vector<pair<CVIBuffer*, CTransform*>>& vecInfo = pRayTestable->GetRayTestTargetInfo();
            for (auto& [pBuffer, pTransform] : vecInfo)
            {
                pRayCaster->RayTest(tHit, vCamPos, vCamLook, pBuffer, pTransform->Get_World());
            }
        }
    }

    if (tHit.bHit)
    {
        m_pHolingObject = dynamic_cast<CLiminalObject*>(tHit.pObject);
        if (m_pHolingObject)
        {
            m_pHolingObject->SetGrabbed(true);
            
            /* 파지 시점의 변환 캡쳐 */
            CTransform* pTransform = m_pHolingObject->GetTransform();
            _matrix matCamInv;
            D3DXMatrixInverse(&matCamInv, nullptr, &matCamWorld);
            m_matCapture = (*pTransform->Get_World()) * matCamInv;
            memcpy(&m_vCaptureDisplacement, &m_matCapture.m[3][0], sizeof(_vec3));
            m_fCaptureDist = D3DXVec3Length(&m_vCaptureDisplacement);
            _vec3 vLook = pTransform->Get_Info_Value(INFO_LOOK);
            m_fCaptureScale = D3DXVec3Length(&vLook); /* 균등 스케일 가정 */
        }
    }
}

void CLiminalGun::CalculateView(CLiminalObject* pObject)
{
    /* */
    CCameraObj* pCamera = CCameraMgr::GetInstance()->GetCamera(L"Camera_Player_FPV");
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
                const multimap<wstring, CGameObject*>& mapObject = pStage->GetCurrentRoomLayer()->Get_ObjMap();
                CRayCaster* pRayCaster = static_cast<CRayCaster*>(CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"RayCaster"));

                for (auto& [wstrName, pObject] : mapObject)
                {
                    if (IRayTestable* pRayTestable = dynamic_cast<IRayTestable*>(pObject))
                    {
                        if (pRayTestable == m_pHolingObject) continue; /* 본인은 통과 */
                         
                        const vector<pair<CVIBuffer*, CTransform*>>& vecInfoDst = pRayTestable->GetRayTestTargetInfo();
                        for (auto& [pBufferDst, pTransformDst] : vecInfoDst)
                        {
                            pRayCaster->RayTest(tHit, vCamPos, vDir, pBufferDst, pTransformDst->Get_World());
                        }
                    }
                }
            }
        }
    }

    if (tHit.bHit)
    {
        float fDist = tHit.fDist;

        float fCurScale = pObject->GetTransform()->Get_Scale().x; /* 균등이니깐 그냥 x만 */
        //float fMargin = fCurScale * sqrtf(3.f) / 2.f;
        float fMargin = 0;

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
