#pragma once

#include "CBase.h"
#include "CLayer.h"

BEGIN(Engine)

class ENGINE_DLL CScene : public CBase
{
protected:
	explicit CScene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene();

public:
	CComponent* Get_Component(COMPONENTID eID,
		const wstring& wstrLayerTag,
		const _tchar* pObjTag,
		const _tchar* pComponentTag);

	CGameObject* Get_GameObject(const wstring& wstrLayerTag, const _tchar* pObjTag);

	virtual HRESULT Add_GameObject(const wstring& pObjTag, CGameObject* pGameObject) PURE;

	CLayer* Get_Layer(const wstring& wstrLayerTag);

public:
	virtual			HRESULT		Ready_Scene();
	virtual			_int		Update_Scene(const _float& fTimeDelta);
	virtual			void		LateUpdate_Scene(const _float& fTimeDelta);
	virtual			void		Render_Scene() PURE;

	// 씬이 CurrentScene으로 지정	될 때 호출되는 함수
	virtual			void		OnEnter() {}
	// 다른 씬으로 넘어가면서 화면에서 사라질 때 호출되는 함수
	virtual			void		OnExit() {}

protected:
	map<wstring, CLayer*>			m_mapLayer;
	LPDIRECT3DDEVICE9				m_pGraphicDev;

protected:
	virtual void			Free();

};

END