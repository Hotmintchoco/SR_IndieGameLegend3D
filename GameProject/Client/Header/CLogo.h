#pragma once

#include "CScene.h"
#include "CLoading.h"
class CGaugeUI;

class CLogo :  public CScene
{
private:
	explicit CLogo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLogo();

public:
	virtual			HRESULT		Ready_Scene();
	virtual			_int		Update_Scene(_float fTimeDelta);
	virtual			void		LateUpdate_Scene(_float fTimeDelta);
	virtual			void		Render_Scene();

	virtual HRESULT Add_GameObject(const wstring& pObjTag, CGameObject* pGameObject) override { return S_OK; }

private:
	HRESULT			Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag)	{ return S_OK; }
	HRESULT			Ready_UI_Layer(const _tchar* pLayerTag);

	HRESULT			Ready_Prototype();

private:
	CLoading*		m_pLoading;
    CGaugeUI*		m_pLoadingGauge = nullptr; // Owned by UI_Layer.
    _float			m_fDisplayProgress = 0.f;
    _float			m_fCompleteHold = 0.f;
    _bool			m_bStageFailed = false;

public:
	static CLogo* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free();

};

