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

	COLLIDER_TYPE	Get_ColliderType() { return m_eColliderType; }

	void			Set_CollisionID(const _int& iCollisionID) { m_iCollisionID = iCollisionID; }
	_int			Get_CollisionID() { return m_iCollisionID; }

	virtual _bool	Intersect(CCollider* pOther) PURE;

	virtual void	OnCollisionEnter(CCollider* pOther);
	virtual void	OnCollisionStay(CCollider* pOther);
	virtual void	OnCollisionExit(CCollider* pOther);

public:
	virtual _int	Update_Component(const _float& fTimeDelta) { return 0; };
	virtual void	LateUpdate_Component() {}

public:
	virtual CComponent* Clone() PURE;

protected:
	virtual void		Free();

protected:
	_bool			m_bIsTrigger;	// 트리거 여부를 나타내는 변수(아직 안씀)
	_bool			m_bIsCollided;	// 충돌 여부를 나타내는 변수
	_bool			m_bIsActive;	// 충돌 체크 활성화 여부를 나타내는 변수
	_float			m_fRadius;

	COLLIDER_TYPE	m_eColliderType = CT_NONE;
	_int			m_iCollisionID = COLL_END;
};

END