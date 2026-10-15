#include "pch.h"
#include "CUI_PhaseClear.h"
#include "CRenderer.h"
#include "CTexture.h"

CUI_PhaseClear::CUI_PhaseClear(LPDIRECT3DDEVICE9 pGraphicDev)
    : CUI(pGraphicDev)
{
}

CUI_PhaseClear::~CUI_PhaseClear()
{
}

HRESULT CUI_PhaseClear::Ready_GameObject()
{
    m_wstrTextureTag = L"Proto_OrthoClear_Texture";

    if (FAILED(__super::Add_Component()))
        return E_FAIL;

    CGameObject::Ready_GameObject();

    m_iFrameCount = m_pTextureCom ? (int)m_pTextureCom->GetCount() : 0;

    /* 화면 가운데 */
    Set_Pos(_vec2{ WINCX * 0.5f, WINCY * 0.5f });
    Set_Size(_vec2{ s_fWidth * 0.5f, s_fHeight * 0.5f });

    /* Play 전까지는 꺼둠 */
    Set_IsActive(false);

    return S_OK;
}

_int CUI_PhaseClear::Update_GameObject(_float fTimeDelta)
{
    /* CUI::Update_GameObject는 스위치 값으로 활성 상태를 다시 켜므로 쓰지 않음 */
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    if (!m_bPlaying)
        return iExit;

    m_fTimeAfterPlay += fTimeDelta;

    /* 마지막 장까지 보여준 뒤 스스로 꺼짐 */
    const int iFrame = (int)(m_fTimeAfterPlay / m_fFrameInterval);
    if (iFrame >= m_iFrameCount)
    {
        m_bPlaying = false;
        Set_IsActive(false);
        m_OnPlayFinished.Broadcast();
        return iExit;
    }

    Set_Texture((_uint)iFrame);
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    return iExit;
}

void CUI_PhaseClear::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CUI_PhaseClear::Render_GameObject()
{
    CUI::Render_GameObject();
}

void CUI_PhaseClear::Play()
{
    if (m_iFrameCount <= 0) return;

    m_bPlaying = true;
    m_fTimeAfterPlay = 0.f;
    Set_Texture(0);
    Set_IsActive(true);
}

CUI_PhaseClear* CUI_PhaseClear::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CUI_PhaseClear* pUI = new CUI_PhaseClear(pGraphicDev);

    if (FAILED(pUI->Ready_GameObject()))
    {
        Safe_Release(pUI);
        MSG_BOX("CUI_PhaseClear Create Failed");
        return nullptr;
    }

    return pUI;
}

void CUI_PhaseClear::Free()
{
    CUI::Free();
}
