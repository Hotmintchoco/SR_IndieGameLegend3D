#include "pch.h"
#include "CRapidGunUltimateMarker.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CMonster.h"

CRapidGunUltimateMarker::CRapidGunUltimateMarker(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CRapidGunUltimateMarker::~CRapidGunUltimateMarker()
{
}

HRESULT CRapidGunUltimateMarker::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    SetSize(192, 192);

    m_fTimeAfterTarget = 0.f;
    m_iTextureIndex = 0;
    m_iTextureFrameCount = m_pTexture->GetCount();

    return S_OK;
}

_int CRapidGunUltimateMarker::Update_GameObject(_float fTimeDelta)
{
    if (!Get_IsActive()) return S_OK;

    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    if (m_bVisible) CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
    
    UpdateScreenPos();

    UpdateAnimation(fTimeDelta);

    return iExit;
}

void CRapidGunUltimateMarker::LateUpdate_GameObject(_float fTimeDelta)
{
    if (!Get_IsActive()) return;

    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CRapidGunUltimateMarker::Render_GameObject()
{
    if (!m_pCurrentTarget || !m_bVisible || !Get_IsActive()) return;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransform->Get_World());
    m_pTexture->Set_Texture(m_iTextureIndex);
    m_pBuffer->Render_Buffer();
}

void CRapidGunUltimateMarker::SetSize(int iX, int iY)
{
    m_pTransform->Set_Scale(_vec3{ iX * 0.5f, iY * 0.5f, 1.f });
}

void CRapidGunUltimateMarker::UpdateTarget(CMonster* pMonster)
{
    if (m_pCurrentTarget == pMonster) return;

    m_pCurrentTarget = pMonster;
    m_iTextureIndex = 0;
    m_fTimeAfterTarget = 0.f;
    m_bVisible = false;
}

void CRapidGunUltimateMarker::UpdateScreenPos()
{
    m_bVisible = false;

    if (!m_pCurrentTarget) return;

    if (m_pCurrentTarget->Is_Dead())
    {
        UpdateTarget(nullptr);
        return;
    }

    _vec3 vPos;
    m_pCurrentTarget->Get_Pos(&vPos);

    /* Get Screen Pos */
    _matrix matView, matProj;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
    m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

    // 카메라 뒤에 있으면 투영 결과가 뒤집히므로 뷰 공간 z로 먼저 거름
    _vec3 vViewPos;
    D3DXVec3TransformCoord(&vViewPos, &vPos, &matView);
    if (vViewPos.z <= 0.f) return;

    // 월드 -> NDC (-1 ~ 1)
    _vec3 vNDC;
    D3DXVec3TransformCoord(&vNDC, &vViewPos, &matProj);

    // 화면 밖이면 그리지 않음 (가장자리에 고정하고 싶으면 여기서 clamp)
    if (vNDC.x < -1.f || vNDC.x > 1.f || vNDC.y < -1.f || vNDC.y > 1.f)
        return;

    // NDC -> 중앙 원점 픽셀 좌표 (Render의 직교 투영과 같은 좌표계)
    D3DVIEWPORT9 tViewport;
    m_pGraphicDev->GetViewport(&tViewport);

    _vec3 vScreen{
        vNDC.x * tViewport.Width * 0.5f,
        vNDC.y * tViewport.Height * 0.5f,
        0.1f };

    m_pTransform->Set_Pos(vScreen);
    m_bVisible = true;
}

void CRapidGunUltimateMarker::UpdateAnimation(float fTimeDelta)
{
    m_fTimeAfterTarget += fTimeDelta;

    if (m_iTextureFrameCount > 0)
        m_iTextureIndex = min(m_iTextureFrameCount - 1, static_cast<int>(m_fTimeAfterTarget / m_fFrameInterval));
}

HRESULT CRapidGunUltimateMarker::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransform = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Buffer
    pComponent = m_pBuffer = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTexture = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Ultimate_RapidGun_Marker_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CRapidGunUltimateMarker* CRapidGunUltimateMarker::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CRapidGunUltimateMarker* pMarker = new CRapidGunUltimateMarker(pGraphicDev);

    if (FAILED(pMarker->Ready_GameObject()))
    {
        Safe_Release(pMarker);
        MSG_BOX("CRapidGunUltimateMarker Create Failed");
        return nullptr;
    }

    return pMarker;
}

void CRapidGunUltimateMarker::Free()
{
    CGameObject::Free();
}