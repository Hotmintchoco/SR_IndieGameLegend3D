#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CTransform;
	class CGameObject;
	class CSphereCollider;
}

class CExplodeRange : public CGameObject
{
protected:
	explicit CExplodeRange(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CExplodeRange(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos);
	virtual ~CExplodeRange();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	inline void AttachTo(Engine::CGameObject* pObject) { m_pParentObject = pObject; }
	void PropagateTransform(CTransform* pTransform);

	/* 구형 콜라이더로 가정 */
	void SetScale(const float fScale);
	inline void SetDelayTime(const float fTime) { m_fLeftCollisionCheckTime = fTime; }
	inline void OnSwitch() { m_bDelayedSwitch = true; }

private:
	HRESULT			Add_Component();

protected:
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CSphereCollider* m_pColliderCom = nullptr;

	Engine::CGameObject* m_pParentObject = nullptr;

	/* 지연 폭발 처리 */
	float m_fLeftCollisionCheckTime = 0.1f;
	bool m_bDelayedSwitch = false;
	virtual	void OnCollisionEnter(COLLINFO eCollInfo) override;

	/* 직접 위치 지정 */
	_vec3 m_vPos{ 0.f, 0.f, 0.f };

public:
	static CExplodeRange* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CExplodeRange* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos);

private:
	virtual void		Free();
};

