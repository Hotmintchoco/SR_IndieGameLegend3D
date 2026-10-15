#include "pch.h"
#include "CUI_OrthoIntro.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CDInputMgr.h"

CUI_OrthoIntro::CUI_OrthoIntro(LPDIRECT3DDEVICE9 pGraphicDev)
    : CUI(pGraphicDev)
{
}

CUI_OrthoIntro::~CUI_OrthoIntro()
{
}

HRESULT CUI_OrthoIntro::Ready_GameObject()
{
    m_wstrTextureTag = L"Proto_OrthoIntro_Texture";

    if (FAILED(__super::Add_Component()))
        return E_FAIL;

    CGameObject::Ready_GameObject();

    m_iFrameCount = m_pTextureCom ? (int)m_pTextureCom->GetCount() : 0;
    /* 텍스처가 덜 들어와 있어도 범위를 넘지 않도록 */
    m_iLoopEnd = min(s_iLoopEnd, m_iFrameCount - 1);

    /* 화면 가운데 */
    Set_Pos(_vec2{ WINCX * 0.5f, WINCY * 0.5f });
    Set_Size(_vec2{ s_fWidth * 0.5f, s_fHeight * 0.5f });

    /* Play 전까지는 꺼둠 */
    Set_IsActive(false);

    return S_OK;
}

_int CUI_OrthoIntro::Update_GameObject(_float fTimeDelta)
{
    /* CUI::Update_GameObject는 스위치 값으로 활성 상태를 다시 켜므로 쓰지 않음 */
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    if (!m_bPlaying)
        return iExit;

    m_fTimeAfterPlay += fTimeDelta;

    int iFrame = (int)(m_fTimeAfterPlay / s_fFrameInterval);
    const bool bLooping = (iFrame >= s_iLoopStart && s_iLoopStart <= m_iLoopEnd);

    /* 루프 구간 : 마지막 장을 넘으면 루프 시작 장으로 돌아감 */
    if (bLooping)
        iFrame = s_iLoopStart + (iFrame - s_iLoopStart) % (m_iLoopEnd - s_iLoopStart + 1);
    else
        iFrame = min(iFrame, m_iFrameCount - 1);

    /* 루프 구간에 들어온 뒤 아무 키나 누르면 종료 */
    if (bLooping && IsAnyKeyDown())
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

void CUI_OrthoIntro::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CUI_OrthoIntro::Render_GameObject()
{
    CUI::Render_GameObject();
}

void CUI_OrthoIntro::Play()
{
    if (m_iFrameCount <= 0) return;

    m_bPlaying = true;
    m_fTimeAfterPlay = 0.f;
    Set_Texture(0);
    Set_IsActive(true);
}

bool CUI_OrthoIntro::IsAnyKeyDown()
{
    CDInputMgr* pInput = CDInputMgr::GetInstance();

    for (int i = 1; i < 256; ++i)
    {
        if (pInput->Key_Down((_ubyte)i))
            return true;
    }

    return false;
}

CUI_OrthoIntro* CUI_OrthoIntro::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CUI_OrthoIntro* pUI = new CUI_OrthoIntro(pGraphicDev);

    if (FAILED(pUI->Ready_GameObject()))
    {
        Safe_Release(pUI);
        MSG_BOX("CUI_OrthoIntro Create Failed");
        return nullptr;
    }

    return pUI;
}

void CUI_OrthoIntro::Free()
{
    CUI::Free();
}
