#include "pch.h"
#include "CIntroScene.h"
#include "CLogo.h"
#include "CManagement.h"
#include "CDInputMgr.h"
#include "CFontMgr.h"
#include "CCursorPolicyMgr.h"
#include "CSoundMgr.h"
#include "CProtoMgr.h"
#include "CUI.h"

CIntroScene::CIntroScene(LPDIRECT3DDEVICE9 pGraphicDev)
    : CScene(pGraphicDev)
{
}

HRESULT CIntroScene::Ready_Scene()
{
    if (FAILED(Ready_Prototype()))
        return E_FAIL;

    if (FAILED(Ready_UI_Layer()))
        return E_FAIL;

    if (FAILED(CSoundMgr::GetInstance()->LoadSound(L"../Bin/Resource/Sound/sfx/")))
        return E_FAIL;

    return S_OK;
}

HRESULT CIntroScene::Ready_Prototype()
{
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_IntroChatWindow", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Intro/ChatWindow.png",1))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_IntroBean", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Intro/bean_%d.png", 2))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_IntroPlayer", CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Intro/player.png", 1))))
        return E_FAIL;

    return S_OK;
}

HRESULT CIntroScene::Ready_UI_Layer()
{
    CLayer* pLayer = CLayer::Create();
    if (!pLayer) 
        return E_FAIL;

    m_mapLayer.insert({ L"Dialogue_Layer", pLayer });

    const float fScale = min(WINCX / 1280.f, WINCY / 720.f);
    const float fLeft = WINCX * 0.5f - 480.f * fScale;
    const float fTop = 112.f * fScale;

    auto* pWindow = CUI::Create(m_pGraphicDev, L"Proto_IntroChatWindow");

    if (!pWindow) 
        return E_FAIL;

    pWindow->Set_Size({ 480.f * fScale, 85.f * fScale });
    pWindow->Set_Pos(WINCX * 0.5f, fTop + 85.f * fScale, 0.9f);
    if (FAILED(pLayer->Add_GameObject(L"ChatWindow", pWindow)))
    {
        Safe_Release(pWindow);
        return E_FAIL;
    }

    m_pPortrait = CUI::Create(m_pGraphicDev, L"Proto_IntroBean");

    if (!m_pPortrait) 
        return E_FAIL;

    m_pPortrait->Set_Size({ 74.f * fScale, 74.f * fScale });
    m_pPortrait->Set_Pos(fLeft + 85.f * fScale, fTop + 85.f * fScale, 0.8f);
    if (FAILED(pLayer->Add_GameObject(L"Commander", m_pPortrait)))
    {
        Safe_Release(m_pPortrait);
        return E_FAIL;
    }

    m_pPlayerPortrait = CUI::Create(m_pGraphicDev, L"Proto_IntroPlayer");
    if (!m_pPlayerPortrait) return E_FAIL;
    m_pPlayerPortrait->Set_Size({ 74.f * fScale, 74.f * fScale });
    m_pPlayerPortrait->Set_Pos(fLeft + 875.f * fScale, fTop + 85.f * fScale, 0.8f);
    m_pPlayerPortrait->Set_IsActive(false);
    if (FAILED(pLayer->Add_GameObject(L"PlayerPortrait", m_pPlayerPortrait)))
    {
        Safe_Release(m_pPlayerPortrait);
        return E_FAIL;
    }

    return S_OK;
}

void CIntroScene::Show_Dialogue(size_t iIndex)
{
    if (iIndex >= m_vecDialogue.size())
    {
        Finish_Intro();
        return;
    }
    m_iDialogueIndex = iIndex;
    const auto& line = m_vecDialogue[iIndex];
    const bool bCommander = line.eSpeaker == SPEAKER::COMMANDER;
    m_pPortrait->Set_IsActive(bCommander);
    m_pPlayerPortrait->Set_IsActive(!bCommander);
    Start_Dialogue(line.strText);
}

void CIntroScene::Start_Dialogue(const std::wstring& strText)
{
    m_strDialogue = strText;
    m_strVisibleDialogue.clear();
    m_vecWordEnds.clear();

    m_iVisibleWordCount = 0;
    m_fTextElapsed = 0.f;

    auto IsSpace = [](wchar_t ch)
        {
            return ch == L' ' || ch == L'\n' || ch == L'\r' || ch == L'\t';
        };

    for (size_t i = 0; i < m_strDialogue.size(); ++i)
    {
        const bool bWordEnd =
            !IsSpace(m_strDialogue[i]) && (i + 1 == m_strDialogue.size() || IsSpace(m_strDialogue[i + 1]));

        if (bWordEnd)
            m_vecWordEnds.push_back(i + 1);
    }
}

void CIntroScene::Update_Dialogue(_float fTimeDelta)
{
    if (m_iVisibleWordCount >= m_vecWordEnds.size())
        return;

    m_fTextElapsed += fTimeDelta;

    bool bWordRevealed = false;

    while (m_fTextElapsed >= m_fWordInterval &&
        m_iVisibleWordCount < m_vecWordEnds.size())
    {
        m_fTextElapsed -= m_fWordInterval;

        const size_t iEnd = m_vecWordEnds[m_iVisibleWordCount];
        ++m_iVisibleWordCount;

        m_strVisibleDialogue = m_strDialogue.substr(0, iEnd);
        bWordRevealed = true;
    }

    if (bWordRevealed)
        CSoundMgr::GetInstance()->PlaySFX(L"sfxEnergy.wav");
}

void CIntroScene::OnEnter()
{
    CCursorPolicyMgr::GetInstance()->Set_MenuMode(true);

    CSoundMgr::GetInstance()->SetSFXVolume(0.3f);

    m_vecDialogue = {
        { SPEAKER::COMMANDER, L"연구 시설에서 오류가 생겼네\n그로 인해 실험체들이 풀려났어" },
        { SPEAKER::PLAYER, L"남아있던 동료는 괜찮나요?" },
        { SPEAKER::COMMANDER, L"아직 안에 갇혀 있어\n구조 신호 이후로 연락이.." },
        { SPEAKER::COMMANDER, L"시간이 많이 지나진 않았으니\n자네가 직접 나서주게." },
        { SPEAKER::PLAYER, L"제가 들어가겠습니다.\n동료를 어떻게 구하죠?" },
        { SPEAKER::COMMANDER, L"동료가 갇힌 장치를 해제하게.\n젬 100개를 투입하면 돼." },
        { SPEAKER::PLAYER, L"알겠습니다.\n동료 구출 후 귀환하겠습니다." },
    };
    Show_Dialogue(0);
}

void CIntroScene::OnExit()
{
    CSoundMgr::GetInstance()->StopBGM();
    CCursorPolicyMgr::GetInstance()->Set_MenuMode(false);
}

_int CIntroScene::Update_Scene(_float fTimeDelta)
{
    auto* pInput = CDInputMgr::GetInstance();
    if (pInput->Key_Down(DIK_RETURN))
        Finish_Intro();
    else if (pInput->Key_Down(DIK_SPACE))
    {
        if (m_bLoadingFailed)
            Finish_Intro();
        else if (m_iVisibleWordCount < m_vecWordEnds.size())
        {
            m_iVisibleWordCount = m_vecWordEnds.size();
            m_strVisibleDialogue = m_strDialogue;
        }
        else
            Show_Dialogue(m_iDialogueIndex + 1);
    }

    if (!m_bFinishRequested)
    {
        Update_Dialogue(fTimeDelta);
        const bool bSpeaking = m_iVisibleWordCount < m_vecWordEnds.size();
        m_pPortrait->Set_Texture(bSpeaking ? static_cast<_uint>((GetTickCount64() / 360) % 2) : 1);
        return CScene::Update_Scene(fTimeDelta);
    }

    m_bFinishRequested = false;
    CLogo* pLogo = CLogo::Create(m_pGraphicDev);
    if (!pLogo)
    {
        m_bLoadingFailed = true;
        return 0;
    }

    return CManagement::GetInstance()->Change_Scene(0, pLogo, true);
}

void CIntroScene::LateUpdate_Scene(_float fTimeDelta)
{
    CScene::LateUpdate_Scene(fTimeDelta);
}

void CIntroScene::Render_Scene()
{
    const float fScale = min(WINCX / 1280.f, WINCY / 720.f);
    const float fLeft = WINCX * 0.5f - 480.f * fScale;
    const bool bPlayer = !m_vecDialogue.empty() &&
        m_vecDialogue[m_iDialogueIndex].eSpeaker == SPEAKER::PLAYER;
    const _vec2 vTitle{ fLeft + (bPlayer ? 45.f : 210.f) * fScale, 142.f * fScale };
    const _vec2 vGuide{ fLeft, 325.f * fScale };

    CFontMgr::GetInstance()->Render_Font(
                             L"Font_Dialogue",
                             m_strVisibleDialogue.c_str(),
                             &vTitle,
                             D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

    CFontMgr::GetInstance()->Render_Font(L"Font_Dialogue",
        m_bLoadingFailed ? L"Loading failed. Press ENTER to retry."
                         : L"SPACE : 다음으로      ENTER : 건너뛰기",
                            &vGuide, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
}

CIntroScene* CIntroScene::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CIntroScene* pScene = new CIntroScene(pGraphicDev);
    if (FAILED(pScene->Ready_Scene()))
    {
        Safe_Release(pScene);
        return nullptr;
    }
    return pScene;
}

void CIntroScene::Free()
{
    CScene::Free();
}
