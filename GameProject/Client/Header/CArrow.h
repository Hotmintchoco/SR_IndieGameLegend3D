#pragma once

#include "CProjectile.h"
#include "CEffect.h"

namespace Engine
{
	class CTexture;
	class CSphereCollider;
}

struct TArrowData : public TProjectileData
{
	float fMaxSpeed = 25.f;
	float fGravityCoef = 6.f;
	_vec3 vInitScale = _vec3{0.5f, 0.5f, 0.75f};
	bool bShowTrail = true;
};

class CCrossBuffer;

class CArrow : public CProjectile
{
protected:
	explicit CArrow(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir, float fShotPower);
	explicit CArrow(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir, float fShotPower, const TArrowData& t);
	virtual ~CArrow();

public:
	virtual	HRESULT Ready_GameObject();
	virtual	_int Update_GameObject(_float fTimeDelta);
	virtual	void LateUpdate_GameObject(_float fTimeDelta);
	virtual	void Render_GameObject();

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;
	virtual void OnCollisionExit(COLLINFO eCollInfo) override;

	///// 261002 재현 
	virtual const _vec3 Get_Projectile_Dir() { return m_vVelocity; }
	void Create_Arrow_Trail();
	virtual void Set_TrailDead();
private:
	CEffect* m_pEffect_Trail = nullptr;
	////////////

private:
	HRESULT	Add_Component();
	void InitTransform();
	void ExertGravity(const float fTimeDelta);
	void SyncTransformToVelocity();
	bool PreciseHitTest(CGameObject* pTarget);

	CCrossBuffer* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CSphereCollider* m_pColliderCom = nullptr;

	/* 초기값 */
	_vec3 m_vStart{ 0.f, 0.f, 0.f };
	_vec3 m_vDir{ 0.f, 0.f, 0.f };

	TArrowData m_tData;

	float m_fSpeed = 0.f;

	/* 중력 관련 */
	_vec3 m_vVelocity{0.f, 0.f, 0.f};

	/* 박혔을 때 정보 */
	_vec3 m_vPrevPos{ 0.f, 0.f, 0.f };
	vector<CGameObject*> m_vecRayTestTarget;
	bool m_bStopped = false;

public:
	static CArrow* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir, float fShotPower);
	static CArrow* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir, float fShotPower, const TArrowData& t);

protected:
	virtual void Free() override;

};

