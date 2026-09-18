#include "pch.h"
#include "CExplosiveFrustum.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CExplosiveFrustumLight.h"
#include "CLayer.h"
#include "CGameStatusMgr.h"
#include "CRoomLayer.h"
#include "CAbstractFactory.h"

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

    SpawnLight();

    return S_OK;
}

_int CExplosiveFrustum::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CFrustum::Update_GameObject(fTimeDelta);
    
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    _vec3   vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    Compute_ViewZ(&vPos);

    return iExit;
}

void CExplosiveFrustum::LateUpdate_GameObject(const _float& fTimeDelta)
{
    m_pLight->PropagateTransform(m_pTransformCom);

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
    CRoomLayer* pLayer = CGameStatusMgr::GetInstance()->GetCurrentRoomLayer();
    
    CGameObject* pObject = nullptr;

    _bool bIsDestroyed = DestroyFrustum(dynamic_cast<CCollider*>(pOther->Get_Component(ID_DYNAMIC, L"Com_Collider")));

    if (bIsDestroyed)
    {
        m_pLight->Set_Dead(true);
        CGameObject* pObject = CAbstractFactory::GetInstance()->CreateRandomItem(this);

        if (pObject)
            pLayer->Add_GameObject(L"Item", pObject);
    }
}

HRESULT CExplosiveFrustum::Add_Component()
{
    CComponent* pComponent = nullptr;

    // PlyTex
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ExplosiveFrustum_Vertex"));

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

void CExplosiveFrustum::SpawnLight()
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
