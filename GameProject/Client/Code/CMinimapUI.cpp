#include "pch.h"
#include "CMinimapUI.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CCameraMgr.h"
#include "CGameStatusMgr.h"
#include "CRoomLoadingMgr.h"

CMinimapUI::CMinimapUI(LPDIRECT3DDEVICE9 pGraphicDev)
    : CUI(pGraphicDev)
{
}


CMinimapUI::~CMinimapUI()
{
}

HRESULT CMinimapUI::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    CGameObject::Ready_GameObject();

    return S_OK;
}

_int CMinimapUI::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    const _float fYawRad = CGameStatusMgr::GetInstance()->GetYaw();

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return iExit;
}

void CMinimapUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    _vec3       vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    CGameObject::Compute_ViewZ(&vPos);
}

void CMinimapUI::Render_GameObject()
{
    RECT rcClip;
	rcClip.left = m_vPos.x - m_vSize.x * 0.5f;
	rcClip.top = m_vPos.y - m_vSize.y * 0.5f;
	rcClip.right = m_vPos.x + m_vSize.x * 0.5f;
	rcClip.bottom = m_vPos.y + m_vSize.y * 0.5f;

	// RECT 영역만큼 그려준다. 범위 밖으로 가면 짤림(Scissor Test)
    m_pGraphicDev->SetScissorRect(&rcClip);
    m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);

    // 플레이어 맵 인덱스
	_int iPlayerRoomIndex = CGameStatusMgr::GetInstance()->GetCurrentRoomIndex();
    
}

HRESULT CMinimapUI::Add_Component()
{
    CComponent* pComponent = nullptr;

    // RcCol
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_MinimapUITexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CMinimapUI* CMinimapUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMinimapUI* pGameUI = new CMinimapUI(pGraphicDev);

    if (FAILED(pGameUI->Ready_GameObject()))
    {
        Safe_Release(pGameUI);
        MSG_BOX("CMinimapUI Create Failed");
        return nullptr;
    }

    return pGameUI;
}

void CMinimapUI::Free()
{
    CGameObject::Free();
}
