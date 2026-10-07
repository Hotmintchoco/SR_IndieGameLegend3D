#include "pch.h"
#include "CBombardArrowSpawner.h"
#include "CStage.h"
#include "CRoomLoadingMgr.h"
#include "CManagement.h"
#include "CRoomLayer.h"
#include "CArrow.h"
#include "CRandomMgr.h"

CBombardArrowSpawner::CBombardArrowSpawner(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CBombardArrowSpawner::~CBombardArrowSpawner()
{
}

HRESULT CBombardArrowSpawner::Ready_GameObject()
{
    return S_OK;
}

_int CBombardArrowSpawner::Update_GameObject(_float fTimeDelta)
{
    int iExit = CGameObject::Update_GameObject(fTimeDelta);

    m_fTimeAfterBirth += fTimeDelta;
    if (m_fTimeAfterBirth >= m_fLifeTime)
    {
        Set_Dead(true);
    }
    
    SpawnArrows(fTimeDelta);

    return iExit;
}

void CBombardArrowSpawner::LateUpdate_GameObject(_float fTimeDelta)
{
}

void CBombardArrowSpawner::SpawnArrows(float fTimeDelta)
{
    CStage* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());
    CRoomLayer* pRoomLayer = pStage ? pStage->GetCurrentRoomLayer() : nullptr;

    if (!pRoomLayer) return;

    const _vec3 vCenter = pRoomLayer->GetCenterPos();
    const _vec3 vHalf = CRoomLoadingMgr::GetInstance()->GetInnerRoomSize() * 0.5f;

    for (int i = 0; i < fTimeDelta * m_iSpawnPerSecond; ++i)
    {
        const float fYaw = CRandomMgr::GetInstance()->GetRandomValue<float>(0.f, D3DX_PI * 2.f);
        const float fTargetX = CRandomMgr::GetInstance()->GetRandomValue<float>(-m_fTargetVariance / 2.f, m_fTargetVariance / 2.f);
        const float fTargetZ = CRandomMgr::GetInstance()->GetRandomValue<float>(-m_fTargetVariance / 2.f, m_fTargetVariance / 2.f);
        const float fPitch = CRandomMgr::GetInstance()->GetRandomValue<float>(m_fMinAngle, m_fMaxAngle);
        const float fPower = CRandomMgr::GetInstance()->GetRandomValue<float>(m_fMinShotPower, m_fMaxShotPower);

        /* 1. 방 둘레 원 위의 생성 위치 */
        const _vec3 vPos{
            vCenter.x + sinf(fYaw) * m_fSpawnDist,
            vCenter.y,
            vCenter.z + cosf(fYaw) * m_fSpawnDist
        };

        /* 2. 방 안 무작위 조준점을 향한 수평 방향 */
        const _vec3 vTarget{
            vCenter.x + vHalf.x * fTargetX,
            vPos.y,
            vCenter.z + vHalf.z * fTargetZ
        };
        _vec3 vFlat = vTarget - vPos;
        D3DXVec3Normalize(&vFlat, &vFlat);

        /* 3. 위쪽으로 pitch를 줘서 포물선 */
        const _vec3 vDir{
            vFlat.x * cosf(fPitch),
            sinf(fPitch),
            vFlat.z * cosf(fPitch)
        };

        TArrowData t;
        t.fMaxSpeed = 60.f;
        t.fGravityCoef = 48.f;
        t.fLifeTime = 10.f;
        t.bShowTrail = false;
        CArrow* pArrow = CArrow::Create(m_pGraphicDev, vPos, vDir, fPower, t);
        if (!pArrow) continue;

        if (FAILED(pStage->Add_GameObject(L"Arrow_" + to_wstring(pArrow->GetProjectileID()), pArrow)))
            Safe_Release(pArrow);
    }
}

CBombardArrowSpawner* CBombardArrowSpawner::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CBombardArrowSpawner* pObject = new CBombardArrowSpawner(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CBombardArrowSpawner Create Failed");
        return nullptr;
    }

    return pObject;
}

void CBombardArrowSpawner::Free()
{
}
