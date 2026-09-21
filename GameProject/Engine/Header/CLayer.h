#pragma once
#include "CBase.h"
#include "CGameObject.h"

BEGIN(Engine)

class ENGINE_DLL CLayer : public CBase
{
protected:
	explicit CLayer();
	virtual ~CLayer();

public:
	CComponent*		Get_Component(COMPONENTID eID, wstring pObjTag,	const _tchar* pComponentTag);

	CGameObject*	Get_GameObject(const wstring& pObjTag);

	HRESULT			Add_GameObject(const wstring& pObjTag, CGameObject* pGameObject);

	const multimap<wstring, CGameObject*>& Get_ObjMap() { return m_mapObject; }

public:
	virtual HRESULT			Ready_Layer();
	virtual _int			Update_Layer(const _float& fTimeDelta);
	virtual void			LateUpdate_Layer(const _float& fTimeDelta);


protected:
	multimap<wstring, CGameObject*>			m_mapObject;

public:
	static CLayer* Create();

protected:
	virtual void	Free();
};

END