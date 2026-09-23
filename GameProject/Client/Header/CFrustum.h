#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CTransform;
	class CBoxCollider;
}

struct TRoomEventCtx;

class CFrustum : public CGameObject
{
protected:
	explicit CFrustum(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFrustum();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	// 충돌	이벤트 처리 함수 (기본적으로 Obstacle_Collision 처리)
	virtual void	OnCollisionStay(CGameObject* pOther) override;

protected:
	HRESULT			Add_Component();

	// 공격에 파괴되는 함수
	_bool			CheckDestroyCondition(CCollider* pOtherCollider);

	virtual void Destroy() {}

	void OnRoomEvent(const TRoomEventCtx& t);

protected:
	Engine::CTransform* m_pTransformCom;
	Engine::CBoxCollider* m_pColliderCom;

protected:
	virtual void		Free();
};

