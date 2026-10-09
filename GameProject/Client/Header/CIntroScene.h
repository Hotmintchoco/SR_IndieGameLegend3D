#pragma once
#include "CScene.h"

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

    // Future dialogue completion and skip both use this entry point.
    void Finish_Intro() { m_bFinishRequested = true; }
    static CIntroScene* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    bool m_bFinishRequested = false;
    bool m_bLoadingFailed = false;
    void Free() override;
};
