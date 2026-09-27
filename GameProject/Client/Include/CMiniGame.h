#pragma once

#include "CScene.h"

class CMiniGame : public CScene
{
private:
	explicit CMiniGame(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMiniGame();

public:
	virtual			HRESULT		Ready_Scene();
	virtual			_int		Update_Scene(const _float& fTimeDelta);
	virtual			void		LateUpdate_Scene(const _float& fTimeDelta);
	virtual			void		Render_Scene();

private:
	HRESULT			Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag) { return S_OK; }
	HRESULT			Ready_UI_Layer(const _tchar* pLayerTag) { return S_OK; }

public:
	static CMiniGame* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free();

};

