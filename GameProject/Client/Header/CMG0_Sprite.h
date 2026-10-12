#pragma once

#include "CGameObject.h"
#include "CMiniGame_Header.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
}

class CMiniGame;

class CMG0_Sprite : public CGameObject
{
protected:
	explicit CMG0_Sprite(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMG0_Sprite();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	const _vec3&	Get_Pos() const { return m_vPos; }
	_float			Get_Scale() const { return m_fScale; }

protected:
	HRESULT			Add_Component();
	CMiniGame*		Get_Scene();
	void			Effect_Explode(const _vec3& vPos, _float fScale);

protected:
	Engine::CRcTex*		m_pBufferCom = nullptr;
	Engine::CTexture*	m_pTextureCom = nullptr;

	_vec3		m_vPos{ 0.f, 0.f, 0.f };
	_float		m_fScale = 1.f;
	_uint		m_iFrame = 0;
	DWORD		m_dwTint = 0xFFFFFFFF;

protected:
	virtual void		Free();
};
