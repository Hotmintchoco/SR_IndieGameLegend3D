#pragma once

#include "CScene.h"

class CMiniGame1 : public CScene
{
private:
	explicit CMiniGame1(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMiniGame1();

public:
	virtual			HRESULT		Ready_Scene();
	virtual			_int		Update_Scene(_float fTimeDelta);
	virtual			void		LateUpdate_Scene(_float fTimeDelta);
	virtual			void		Render_Scene();

	virtual HRESULT Add_GameObject(const wstring& pObjTag, CGameObject* pGameObject) override { return S_OK; }

private:
	HRESULT			Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag) { return S_OK; }
	HRESULT			Ready_UI_Layer(const _tchar* pLayerTag) { return S_OK; }

public:
	static CMiniGame1* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free();

};

