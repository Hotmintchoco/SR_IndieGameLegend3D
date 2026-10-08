#pragma once

#include "CGameObject.h"
#include "Client_Struct.h"

namespace Engine
{
	class CTransform;
}
class CPlayerCamera;

class CParticle : public CGameObject
{
protected:
	explicit CParticle(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CParticle();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	HRESULT			Add_Component();

protected:
	Engine::CTransform* m_pTransformCom = nullptr;
	_float				m_fFrame = 0.f;

	_float m_fLifeTime = 0.f;
	_float m_fElapsedTime = 0.f;

	_float m_fScale = 0.f;
	CPlayerCamera* m_pBillBoardCamera = nullptr;

public:
	static CParticle* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	void Set_Pos(_vec3 vPos);
	void Set_Pos(_float fX, _float fY, _float fZ);
	void Set_Scale(_vec3 vPos);
	void Set_Scale(_float fX, _float fY, _float fZ);

	void Set_LifeTime(const _float& fLifeTime) { m_fLifeTime = fLifeTime; }

	void LookAtPlayer();
	void LookAtPlayer2();
public:

protected:
	virtual void		Free();
};

