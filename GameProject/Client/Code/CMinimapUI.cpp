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
    rcClip.left = LONG(m_vPos.x - m_vSize.x * 0.5f);
	rcClip.top = LONG(m_vPos.y - m_vSize.y * 0.5f);
	rcClip.right = LONG(m_vPos.x + m_vSize.x * 0.5f);
	rcClip.bottom = LONG(m_vPos.y + m_vSize.y * 0.5f);

	// RECT 영역만큼 그려준다. 범위 밖으로 가면 짤림(Scissor Test)
    m_pGraphicDev->SetScissorRect(&rcClip);
    m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);

    // 플레이어 맵 인덱스
	_int iPlayerRoomIndex = CGameStatusMgr::GetInstance()->GetCurrentRoomIndex();
	_int pX = iPlayerRoomIndex % 5;
	_int pY = iPlayerRoomIndex / 5;

    for (_int i = 0; i < CRoomLoadingMgr::GetInstance()->GetRoomTotalCount(); ++i)
    {
        _int x = i % 5;
        _int y = i / 5;
        
        TRoomData* pRoomData = CRoomLoadingMgr::GetInstance()->GetRoomData(i);
        _uint iDoorMask = 0;

        // vecDoorInfo 순서: 동(1), 남(2), 서(4), 북(8)
        if (pRoomData->vecDoorInfo.size() > 0 && pRoomData->vecDoorInfo[0])
            iDoorMask |= 1;
        if (pRoomData->vecDoorInfo.size() > 1 && pRoomData->vecDoorInfo[1])
            iDoorMask |= 2;
        if (pRoomData->vecDoorInfo.size() > 2 && pRoomData->vecDoorInfo[2])
            iDoorMask |= 4;
        if (pRoomData->vecDoorInfo.size() > 3 && pRoomData->vecDoorInfo[3])
            iDoorMask |= 8;

        _float fSize = 22.f;

        _vec3 vPos;
        vPos.x = m_vPos.x + (x - pX) * fSize * 2;
        vPos.y = m_vPos.y + (y - pY) * fSize * 2;
        vPos.z = 0.f;

        m_pTransformCom->Set_Pos(vPos.x - WINCX * 0.5f, -vPos.y + WINCY * 0.5f, vPos.z);
        m_pTransformCom->Set_Scale(fSize, fSize, 1.f);
        m_pTransformCom->Update_Component(0.f);

        m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
		m_pTextureCom->Set_Texture(iDoorMask);
        m_pBufferCom->Render_Buffer();
	}

    // Scissor Test 끄기 (다른 UI에 영향 안 주도록 복구)
    m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
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
