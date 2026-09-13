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

	CLayer* Get_Layer(const wstring& wstrLayerTag);

public:
	virtual			HRESULT		Ready_Scene();
	virtual			_int		Update_Scene(const _float& fTimeDelta);
	virtual			void		LateUpdate_Scene(const _float& fTimeDelta);
	virtual			void		Render_Scene()PURE;

protected:
	map<wstring, CLayer*>			m_mapLayer;
	LPDIRECT3DDEVICE9					m_pGraphicDev;

protected:
	virtual void			Free();

};

END