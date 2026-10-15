#pragma once

#include "CGameObject.h"
#include "CMiniGame_Header.h"

namespace Engine
{
	class CPlyTex;
	class CTexture;
	class CTransform;
}

class CMG0_Train13 : public CGameObject
{
protected:
	explicit CMG0_Train13(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMG0_Train13();

public:
	virtual			HRESULT		Ready_GameObject(_int iLane, _float fSpeed);
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	const _vec3&	Get_Pos() const { return m_vPos; }
	void			Explode();

private:
	HRESULT			Add_Component();
	void			Collision_Player();

private:
	Engine::CPlyTex*	m_pBufferCom = nullptr;
	Engine::CTransform*	m_pTransformCom = nullptr;
	Engine::CTexture*	m_pTextureCom = nullptr;

	_vec3		m_vPos{ 0.f, 0.f, 0.f };
	_float		m_fSpeed = 0.f;

public:
	static CMG0_Train13* Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iLane, _float fSpeed);

protected:
	virtual void		Free();
};
