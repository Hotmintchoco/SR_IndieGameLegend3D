#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
}

class CPlayer : public CGameObject
{
#define	GRAVCONST		60.f

protected:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();
	void			Key_Input(const _float& fTimeDelta);
	void			Mouse_Move();
	void			Mouse_Fix();
	void			Set_OnTerrain(const _float& fTimeDelta);
	_vec3			Picking_OnTerrain();

private:
	Engine::CRcTex*				m_pBufferCom;
	Engine::CTransform*			m_pTransformCom;
	Engine::CTexture*			m_pTextureCom;
	Engine::CCalculator*		m_pCalculatorCom;

private:

	JUMPSTATE	m_iJumpState;
	_float		m_fJumpTime;
	_bool		m_bFix;
	_bool		m_bCheck;

public : 
	JUMPSTATE	Get_JumpState() { return m_iJumpState; }

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

