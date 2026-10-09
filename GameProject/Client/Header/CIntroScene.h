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
    HRESULT Ready_Prototype();
    HRESULT Ready_UI_Layer();
    CUI* m_pPortrait = nullptr; // Owned by the UI layer.
    bool m_bFinishRequested = false;
    bool m_bLoadingFailed = false;

private:
    void Free() override;
};
