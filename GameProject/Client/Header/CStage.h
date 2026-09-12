#pragma once

#include "CScene.h"
#include "Define.h"

class CRoom;

class CStage : public CScene
{
private:
	explicit CStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage();

public:
	virtual			HRESULT		Ready_Scene();
	HRESULT PostInitialize();
	virtual			_int		Update_Scene(const _float& fTimeDelta);
	virtual			void		LateUpdate_Scene(const _float& fTimeDelta);
	virtual			void		Render_Scene();

private:
	HRESULT			Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_UI_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_Light();

	vector<CRoom*> m_vecRoom;

public:
	static CStage* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	_float			m_fLastShotTime;
	_float			m_fReloadTime;
	_int			m_iAmmo;

private:
	virtual void	Free();

};

