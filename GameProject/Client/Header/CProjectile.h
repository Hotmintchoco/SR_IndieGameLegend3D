#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CTransform;
}

class CProjectile : public CGameObject
{
protected:
	explicit CProjectile(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CProjectile();

public:
	virtual	HRESULT Ready_GameObject();
	virtual	_int Update_GameObject(const _float& fTimeDelta);
	virtual	void LateUpdate_GameObject(const _float& fTimeDelta);
	virtual	void Render_GameObject() PURE;

	virtual _uint GetProjectileID() { return m_iID; }

protected:
	HRESULT	Add_Component();
	virtual void CheckLifeTime(const _float& fTimeDelta);
	void BillBoard();

	Engine::CTransform* m_pTransformCom = nullptr;

	/* 기본 속성 */
	float m_fSpeed = 3.f;
	float m_fLifeTime = 1.5f;
	float m_fLeftLifeTime = m_fLifeTime;

	/* 이름 구분용 ID */
	static _uint g_iProjectileID;
	_uint m_iID = -1;

protected:
	virtual void Free() override;
};

