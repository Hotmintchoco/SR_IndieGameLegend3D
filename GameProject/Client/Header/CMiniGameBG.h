#pragma once

#include "CGameObject.h"
#include "Client_Enum.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CMiniGameBG : public CGameObject
{
protected:
	explicit CMiniGameBG(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMiniGameBG();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Ready_GameObject(pair<_int, _int> pIdx);
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

public:
	static	CMiniGameBG* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static	CMiniGameBG* Create(LPDIRECT3DDEVICE9 pGraphicDev, pair<_int, _int> pIdx);

public:
	static const _float	TILE_SIZE;
	static const _float	SCROLL_SPEED;
	static const _int	TILE_MIN_X = -6;
	static const _int	TILE_MAX_X = 6;
	static const _int	TILE_MIN_Z = -3;
	static const _int	TILE_MAX_Z = 19;

protected:
	virtual void		Free();

private :
	CRcTex*		m_pBufferCom = nullptr;
	CTransform*	m_pTransformCom = nullptr;
	CTexture*	m_pTextureCom = nullptr;

	_vec3		m_vPos{ 0.f, 0.f, 0.f };
};
