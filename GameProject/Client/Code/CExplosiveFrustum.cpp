#include "pch.h"
#include "CExplosiveFrustum.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CExplosiveFrustumLight.h"
#include "CExplosiveFrustumGlass.h"
#include "CLayer.h"
#include "CGameStatusMgr.h"
#include "CRoomLayer.h"
#include "CAbstractFactory.h"
#include "CFrustumExplodeEffect.h"
#include "CCollisionMgr.h"
#include "CExplodeRange.h"
#include "CRandomMgr.h"
#include "CExplodeSphere.h"
#include "CSoundMgr.h"

CExplosiveFrustum::CExplosiveFrustum(LPDIRECT3DDEVICE9 pGraphicDev)
    : CFrustum(pGraphicDev)
{
}

CExplosiveFrustum::~CExplosiveFrustum()
{
}

HRESULT CExplosiveFrustum::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    // Note : 순서에 주의
    if (FAILED(CFrustum::Ready_GameObject()))
        return E_FAIL;

    SpawnChildren();

    return S_OK;
}

_int CExplosiveFrustum::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CFrustum::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    _vec3   vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    Compute_ViewZ(&vPos);

    return iExit;
}

void CExplosiveFrustum::LateUpdate_GameObject(const _float& fTimeDelta)
{
    m_pLight->PropagateTransform(m_pTransformCom);
    m_pGlass->PropagateTransform(m_pTransformCom);
    m_pExplodeRange->PropagateTransform(m_pTransformCom);

    CFrustum::LateUpdate_GameObject(fTimeDelta);
}

void CExplosiveFrustum::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture(0);

    m_pBufferCom->Render_Buffer();
}

void CExplosiveFrustum::OnCollisionEnter(CGameObject* pOther)
{
    _bool bIsDestroyed = CheckDestroyCondition(dynamic_cast<CCollider*>(pOther->Get_Component(ID_DYNAMIC, L"Com_Collider")));

    if (bIsDestroyed)
    {
        Destroy();
    }
}

HRESULT CExplosiveFrustum::Add_Component()
{
    CComponent* pComponent = nullptr;

    // PlyTex
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ExplosiveFrustum_Bottom_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ExplosiveFrustum_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CExplosiveFrustum::SpawnChildren()
{
    CExplosiveFrustumLight* pLight = CExplosiveFrustumLight::Create(m_pGraphicDev);

    if (!pLight)
    {
        assert(0);
        return;
    }

    m_pLight = pLight;
    pLight->AttachTo(this);
    /* 아마 이름은 중복이 여럿 될 것. 일단 스폰만 확인 */
    m_pOwner->Add_GameObject(L"Explosive_Frustum_Light", pLight);

    CExplosiveFrustumGlass* pGlass = CExplosiveFrustumGlass::Create(m_pGraphicDev);

    if (!pGlass)
    {
        assert(0);
        return;
    }

    m_pGlass = pGlass;
    pGlass->AttachTo(this);
    /* 아마 이름은 중복이 여럿 될 것. 일단 스폰만 확인 */
    m_pOwner->Add_GameObject(L"Explosive_Frustum_Glass", pGlass);

    CExplodeRange* pArea = CExplodeRange::Create(m_pGraphicDev);

    if (!pArea)
    {
        assert(0);
        return;
    }

    m_pExplodeRange = pArea;
    pArea->AttachTo(this);
    /* 아마 이름은 중복이 여럿 될 것. 일단 스폰만 확인 */
    m_pOwner->Add_GameObject(L"Explode_Range", pArea);

    m_pExplodeRange->SetScale(2.f);
    m_pExplodeRange->SetDelayTime(
        CRandomMgr::GetInstance()->AddRandomNoise<float>(m_fPropagateSpeed, m_fPropagateVariance));
}

void CExplosiveFrustum::Destroy()
{
    CRoomLayer* pLayer = CGameStatusMgr::GetInstance()->GetCurrentRoomLayer();

    /* 자식 오브젝트 삭제 처리 */
    m_pLight->Set_Dead(true);
    m_pGlass->Set_Dead(true);

    /* 지연 폭파 카운트 시작 */
    m_pExplodeRange->OnSwitch();

    /* 아이템 */
    CGameObject* pObject = CAbstractFactory::GetInstance()->CreateRandomItem(this);
    if (pObject)
        pLayer->Add_GameObject(L"Item", pObject);

    /* 폭발 효과 */
    _vec3 vPosNoise = _vec3{
    CRandomMgr::GetInstance()->GetRandomValue<float>(-0.1f, 0.1f),
    CRandomMgr::GetInstance()->GetRandomValue<float>(0.4f, 0.6f),
    CRandomMgr::GetInstance()->GetRandomValue<float>(-0.1f, 0.1f),
    };
    const float fScaleNoise = CRandomMgr::GetInstance()->AddRandomNoise<float>(0.7f, 0.2f);
    pObject = CFrustumExplodeEffect::Create(m_pGraphicDev, m_pTransformCom->m_vInfo[INFO_POS] + vPosNoise, _vec3{ fScaleNoise, fScaleNoise, fScaleNoise });
    if (nullptr == pObject)
        assert(0);
    if (FAILED(pLayer->Add_GameObject(L"FrustumExplode", pObject)))
        assert(0);
    
    /* 폭발 효과 (구) */
    vPosNoise = _vec3{
        CRandomMgr::GetInstance()->GetRandomValue<float>(-0.2f, 0.2f),
        CRandomMgr::GetInstance()->GetRandomValue<float>(0.2f, 0.4f),
        CRandomMgr::GetInstance()->GetRandomValue<float>(-0.2f, 0.2f),
    };
    const float fStartScale = CRandomMgr::GetInstance()->AddRandomNoise<float>(0.2f, 1.f);
    const float fEndScale = CRandomMgr::GetInstance()->AddRandomNoise<float>(1.f + fStartScale, 0.6f);
    const float fLifeTime = CRandomMgr::GetInstance()->GetRandomValue<float>(0.05f, 0.2f);
    pObject = CExplodeSphere::Create(m_pGraphicDev, m_pTransformCom->m_vInfo[INFO_POS] + vPosNoise, fStartScale, fEndScale, fLifeTime);
    if (nullptr == pObject)
        assert(0);
    if (FAILED(pLayer->Add_GameObject(L"ExplodeSphere", pObject)))
        assert(0);
    
    /* 폭발 사운드 */
    CSoundMgr::GetInstance()->PlaySFX(L"sfxExplode.wav");

    /* 방 깜빡임 */
    if (CRandomMgr::GetInstance()->Chance(m_fFlickerChance))
    {
        static_cast<CRoomLayer*>(m_pOwner)->FlickerLight(m_fFlickerDuration);
    }

    Set_Dead(true);
}

CExplosiveFrustum* CExplosiveFrustum::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CExplosiveFrustum* pFrustum = new CExplosiveFrustum(pGraphicDev);

    if (FAILED(pFrustum->Ready_GameObject()))
    {
        Safe_Release(pFrustum);
        MSG_BOX("CExplosiveFrustum Create Failed");
        return nullptr;
    }

    return pFrustum;
}

void CExplosiveFrustum::Free()
{
    CFrustum::Free();
}
