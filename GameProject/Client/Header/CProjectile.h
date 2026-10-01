#pragma once

#include "CGameObject.h"
class CBullet_Trail;

namespace Engine
{
	class CTransform;
}

struct TProjectileData
{
	float fSpeed = 20.f;
	float fLifeTime = 1.5f;
};

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

	//261001 재현
public:
	CTransform* Get_Transform() { return m_pTransformCom; }
	const _vec3 Get_Projectile_Dir() { return m_vDir; }
	void Set_TrailPointer(CGameObject* pTrail) { m_pTrail = pTrail; }
	void Create_Trail();
	void Create_BulletDead_Effect();
	void Set_TrailDead();
	//261001

protected:
	HRESULT	Add_Component();
	virtual void CheckLifeTime(const _float& fTimeDelta);
	void BillBoard();

	Engine::CTransform* m_pTransformCom = nullptr;

	const TProjectileData* m_pData = nullptr;
	float m_fTimeAfterBirth = 0.f;

	/* 초기값 */
	_vec3 m_vStart{ 0.f, 0.f, 0.f };
	_vec3 m_vDir{ 0.f, 0.f, 0.f };

	/* 이름 구분용 ID */
	static _uint g_iProjectileID;
	_uint m_iID = -1;


	CGameObject* m_pTrail = nullptr;

protected:
	virtual void Free() override;
};

