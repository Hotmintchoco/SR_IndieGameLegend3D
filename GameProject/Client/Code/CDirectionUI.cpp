#include "pch.h"
#include "CDirectionUI.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CCameraMgr.h"
#include "CGameStatusMgr.h"

CDirectionUI::CDirectionUI(LPDIRECT3DDEVICE9 pGraphicDev)
    : CUI(pGraphicDev), m_fCompassUOffset(0.f)
{
}


CDirectionUI::~CDirectionUI()
{
}

HRESULT CDirectionUI::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    CGameObject::Ready_GameObject();

    // 나침반 바 형태에 맞게 가로를 길게
    m_pTransformCom->Set_Scale(60.f, 24.f, 1.f);

    return S_OK;
}

_int CDirectionUI::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

	const _float fYawRad = CGameStatusMgr::GetInstance()->GetYaw();

    // [-pi, pi] -> [0, 1)
    m_fCompassUOffset = fYawRad / (2.f * D3DX_PI) + 0.06f;
    if (m_fCompassUOffset < 0.f)
        m_fCompassUOffset += 1.f;

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return iExit;
}

void CDirectionUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    _vec3       vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    CGameObject::Compute_ViewZ(&vPos);
}

void CDirectionUI::Render_GameObject()
{
    // 이전의 텍스처 좌표 변환 상태를 저장
    _matrix matOldTex;
    DWORD dwOldTTFF = D3DTTFF_DISABLE;
    DWORD dwOldAddressU = D3DTADDRESS_CLAMP;

    m_pGraphicDev->GetTransform(D3DTS_TEXTURE0, &matOldTex);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, &dwOldTTFF);
    m_pGraphicDev->GetSamplerState(0, D3DSAMP_ADDRESSU, &dwOldAddressU);

    const _float fVisibleURange = 0.35f; // 크기 조절

    _float fCenteredOffset = m_fCompassUOffset - (fVisibleURange * 0.5f);
    while (fCenteredOffset < 0.f) fCenteredOffset += 1.f;
    while (fCenteredOffset >= 1.f) fCenteredOffset -= 1.f;

    _matrix matTex;
    D3DXMatrixIdentity(&matTex);
    matTex._11 = fVisibleURange;   // U 스케일 축소(잘라내기)
    matTex._31 = fCenteredOffset;  // 현재 방향 기준으로 윈도우 이동

    // 텍스처 UV 변환을 적용
    m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &matTex);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);

    CUI::Render_GameObject();

    // 이전의 텍스처 좌표 변환 상태를 복원
    m_pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSU, dwOldAddressU);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, dwOldTTFF);
    m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &matOldTex);
}

HRESULT CDirectionUI::Add_Component()
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
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_DirectionUITexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CDirectionUI* CDirectionUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CDirectionUI* pGameUI = new CDirectionUI(pGraphicDev);

    if (FAILED(pGameUI->Ready_GameObject()))
    {
        Safe_Release(pGameUI);
        MSG_BOX("CDirectionUI Create Failed");
        return nullptr;
    }

    return pGameUI;
}

void CDirectionUI::Free()
{
    CGameObject::Free();
}
