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
    , m_vWindowSize(0.f, 0.f)
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
    DWORD dwZEnable = TRUE;
    DWORD dwZWrite = TRUE;
    DWORD dwScissor = FALSE;
    RECT rcOld = {};

    m_pGraphicDev->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
    m_pGraphicDev->GetRenderState(D3DRS_ZWRITEENABLE, &dwZWrite);
    m_pGraphicDev->GetRenderState(D3DRS_SCISSORTESTENABLE, &dwScissor);
    m_pGraphicDev->GetScissorRect(&rcOld);

    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

    const float fHalfW = m_vWindowSize.x * 0.5f;
    const float fHalfH = m_vWindowSize.y * 0.5f;
    RECT rcWindow = {
        (LONG)(m_vPos.x - fHalfW),
        (LONG)(m_vPos.y - fHalfH),
        (LONG)(m_vPos.x + fHalfW),
        (LONG)(m_vPos.y + fHalfH)
    };
    m_pGraphicDev->SetScissorRect(&rcWindow);
    m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);

    CRoomLoadingMgr* pRoomMgr = CRoomLoadingMgr::GetInstance();
    CGameStatusMgr* pStatus = CGameStatusMgr::GetInstance();

    const int iColCount = pRoomMgr->GetRoomColCount();
    const int iPlayer = pStatus->GetCurrentRoomIndex();
    const int iPlayerCol = iPlayer % iColCount;
    const int iPlayerRow = iPlayer / iColCount;
    const float fCell = 8.f;

    for (int i = 0; i < pRoomMgr->GetRoomTotalCount(); ++i)
    {
        if (!pStatus->IsVisited(i))
            continue;

        TRoomData* pRoom = pRoomMgr->GetRoomData(i);
        const _uint iFrame =
            (pRoom->vecDoorInfo[0] ? 1u : 0u) |
            (pRoom->vecDoorInfo[1] ? 2u : 0u) |
            (pRoom->vecDoorInfo[2] ? 4u : 0u) |
            (pRoom->vecDoorInfo[3] ? 8u : 0u);

        const int iCol = i % iColCount;
        const int iRow = i / iColCount;

        Set_Size({ fCell, fCell });
        Set_Pos(
            m_vPos.x + (iCol - iPlayerCol) * fCell,
            m_vPos.y + (iRow - iPlayerRow) * fCell,
            0.f);
        Set_Texture(iFrame);
        CUI::Render_GameObject();
    }

    m_pGraphicDev->SetScissorRect(&rcOld);
    m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, dwScissor);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, dwZWrite);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, dwZEnable);
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
