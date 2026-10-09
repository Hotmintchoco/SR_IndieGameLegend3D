#pragma once
#include "CScene.h"
class CUI;

class CIntroScene : public CScene
{
private:
    explicit CIntroScene(LPDIRECT3DDEVICE9 pGraphicDev);
    ~CIntroScene() override = default;

public:
    HRESULT     Ready_Scene() override;
    _int        Update_Scene(_float fTimeDelta) override;
    void        LateUpdate_Scene(_float fTimeDelta) override;
    void        Render_Scene() override;

public:
    void        OnEnter() override;
    void        OnExit() override;
    HRESULT     Add_GameObject(const wstring&, CGameObject*) override { return E_NOTIMPL; }

    void                Finish_Intro() { m_bFinishRequested = true; }
    static CIntroScene* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    HRESULT     Ready_Prototype();
    HRESULT     Ready_UI_Layer();

    void        Start_Dialogue(const std::wstring& strText);
    void        Update_Dialogue(_float fTimeDelta);

private:
    CUI* m_pPortrait = nullptr; // Owned by the UI layer.
    bool m_bFinishRequested = false;
    bool m_bLoadingFailed = false;

    // 대화
    std::wstring m_strDialogue = L"임무에 대한 설명은 들었나?";
    std::wstring m_strVisibleDialogue;

    // 각 단어가 끝나는 문자열 위치
    std::vector<size_t> m_vecWordEnds;

    size_t m_iVisibleWordCount = 0;

    _float m_fTextElapsed = 0.f;
    _float m_fWordInterval = 0.35f;

private:
    void Free() override;
};
