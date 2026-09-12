#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CTransform;
}

class CRoom : public CGameObject
{
protected:
	explicit CRoom(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CRoom(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex);
	virtual ~CRoom();

public:
	virtual			HRESULT		Ready_GameObject();
	HRESULT PostInitialize();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	inline _vec3 GetCenterPos() { return m_vCenterPos; }

private:
	HRESULT			Add_Component();

private:
	void UpdateCenterPosition();

	Engine::CTransform* m_pTransformCom;

	_vec3 m_vCenterPos = { -99999.f, -99999.f, -99999.f };
	int m_iIndex = - 1;

public:
	static CRoom* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex);

private:
	virtual void		Free();
};

