#pragma once
#include "CScene.h"

class CStartScreen : public CScene
{
private:
    explicit CStartScreen(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CStartScreen() = default;

public:
    virtual			HRESULT		Ready_Scene() override;
    virtual			_int		Update_Scene(_float fTimeDelta) override;
    virtual			void		LateUpdate_Scene(_float fTimeDelta) override;
    virtual			void		Render_Scene() override;

public:
    virtual         void        OnEnter() override;
    virtual         void        OnExit() override;
    virtual         HRESULT     Add_GameObject(const wstring&, CGameObject*) override { return S_OK; }

private:
    HRESULT			Ready_Prototype();
    HRESULT         Ready_UI_Layer(const _tchar* pLayerTag);


private:
    bool m_bStartRequested = false;
    bool m_bStartFailed = false;

public:
    static CStartScreen* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    void Free() override;
};
