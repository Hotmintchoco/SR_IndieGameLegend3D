#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CPlyTex;
	class CTransform;
	class CTexture;
}

class CGun : public CGameObject
{
protected:
	explicit CGun(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGun();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();


private:
	HRESULT			Add_Component();
	void			RenderImGui();

	pair<_vec3, _vec3> Get_MouseRay();

	_vec3			Picking(_vec3 RayPos, _vec3 RayDir, CGameObject* pGameObject);

private:
	Engine::CPlyTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

public:
	static CGun* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	BULLETID	m_iCurBullet;
	_int		m_iDmg;

private:
	_float			m_fLastShotTime;
	_float			m_fShootRate;
	_float			m_fRunningTime;
	_float			m_fLastSkillTime;
	_float			m_fSkillDuration;
	_float			m_fSkillCoolTime;
	_bool			m_bSkillActivated;

private:
	virtual void		Free();
};

