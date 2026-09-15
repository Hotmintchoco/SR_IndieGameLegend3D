#pragma once
#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CCollider : public CComponent
{
protected:
	enum COLLIDER_TYPE { CT_BOX, CT_SPHERE, CT_NONE };

	explicit CCollider();
	explicit CCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider();

public:
	void	Set_IsTrigger(const bool& bIsTrigger) { m_bIsTrigger = bIsTrigger; }
	_bool	Get_IsTrigger() { return m_bIsTrigger; }

	void	Set_IsCollided(const bool& bIsCollided) { m_bIsCollided = bIsCollided; }
	_bool	Get_IsCollided() { return m_bIsCollided; }

	void	Set_IsActive(const bool& bIsActive) { m_bIsActive = bIsActive; }
	_bool	Get_IsActive() { return m_bIsActive; }

	virtual void	Set_Radius(const _float& fRadius) {}
	_float			Get_Radius() { return m_fRadius; }

	_bool	Get_IsPos() { return m_bIsPos; }

	COLLIDER_TYPE	Get_ColliderType() { return m_eColliderType; }

	virtual _bool	Intersect(CCollider* pOther) PURE;

public:
	virtual _int	Update_Component(const _float& fTimeDelta) { return 0; };
	virtual void	LateUpdate_Component() {}


public:
	virtual CComponent* Clone() PURE;

protected:
	virtual void		Free();

protected:
	_bool			m_bIsTrigger;
	_bool			m_bIsPos;
	_bool			m_bIsCollided;
	_bool			m_bIsActive;
	_float			m_fRadius;
	COLLIDER_TYPE	m_eColliderType = CT_NONE;
};

END