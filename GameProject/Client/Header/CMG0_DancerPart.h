#pragma once

#include "CGameObject.h"
#include "CMiniGame_Header.h"

namespace Engine
{
	class CTransform;
}

class CPlayerPartTex;

class CMG0_DancerPart : public CGameObject
{
protected:
	explicit CMG0_DancerPart(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMG0_DancerPart();

public:
	virtual			HRESULT		Ready_GameObject(const _tchar* pProtoTag);
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	virtual _float	Get_PoseAngle(_int iPose) { return 0.f; }
	virtual _float	Get_SitAngle() { return 0.f; }

	void			Set_Parent(CMG0_DancerPart* pParent);
	void			Update_Part(const MINIGAME0::MGDANCEBEAT& tBeat, _float fTimeDelta, const _matrix& matRoot);

	Engine::CTransform*	Get_Transform() { return m_pTransformCom; }
	const _vec3&	Get_Pivot() const { return m_vPivot; }

protected:
	HRESULT			Add_Component(const _tchar* pProtoTag);

protected:
	CPlayerPartTex*		m_pBufferCom = nullptr;
	Engine::CTransform*	m_pTransformCom = nullptr;
	CMG0_DancerPart*	m_pParent = nullptr;

	_vec3		m_vPivot{ 0.f, 0.f, 0.f };
	_float		m_fAngle = 0.f;

protected:
	virtual void		Free();
};
