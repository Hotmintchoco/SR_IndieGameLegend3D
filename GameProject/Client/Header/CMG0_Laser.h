#pragma once

#include "CGameObject.h"
#include "CMiniGame_Header.h"

namespace Engine
{
	class CRcTex;
}

class CMG0_Laser : public CGameObject
{
protected:
	explicit CMG0_Laser(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMG0_Laser();

public:
	virtual			HRESULT		Ready_GameObject(const _vec3& vPos, _float fRadius, _float fWarnTime, _float fFireTime);
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();
	void			Collision_Player();
	void			Render_Beam(_float fRadius, DWORD dwColor);
	_bool			Is_Fire() const { return m_fTime >= m_fWarnTime; }

private:
	Engine::CRcTex*		m_pBufferCom = nullptr;

	_vec3		m_vPos{ 0.f, 0.f, 0.f };
	_float		m_fRadius = 0.f;
	_float		m_fWarnTime = 0.f;
	_float		m_fFireTime = 0.f;
	_float		m_fTime = 0.f;

public:
	static CMG0_Laser* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, _float fRadius, _float fWarnTime, _float fFireTime);

protected:
	virtual void		Free();
};
