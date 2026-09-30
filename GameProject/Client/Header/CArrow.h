#pragma once

#include "CProjectile.h"

namespace Engine
{
	class CTexture;
	class CSphereCollider;
}

struct TArrowData : public TProjectileData
{
	float fMaxSpeed = 20.f;
	float fGravityCoef = 6.f;
	_vec3 vInitScale = _vec3{0.33f, 0.33f, 1.f};
};

class CCrossBuffer;

class CArrow : public CProjectile
{
protected:
	explicit CArrow(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir, float fShotPower);
	virtual ~CArrow();

public:
	virtual	HRESULT Ready_GameObject();
	virtual	_int Update_GameObject(const _float& fTimeDelta);
	virtual	void LateUpdate_GameObject(const _float& fTimeDelta);
	virtual	void Render_GameObject();

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;
	virtual void OnCollisionExit(COLLINFO eCollInfo) override;

private:
	HRESULT	Add_Component();
	void InitTransform();
	void ExertGravity(const float fTimeDelta);
	void SyncTransformToVelocity();

	CCrossBuffer* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CSphereCollider* m_pColliderCom = nullptr;

	/* 초기값 */
	_vec3 m_vStart{ 0.f, 0.f, 0.f };
	_vec3 m_vDir{ 0.f, 0.f, 0.f };

	inline static TArrowData s_tData = []()-> TArrowData {
		TArrowData t;
		t.fLifeTime = 10.f;
		t.fSpeed = 0.f; /* 안씀 */
		return t;
		}();

	float m_fSpeed = 0.f;

	/* 중력 관련 */
	_vec3 m_vVelocity{0.f, 0.f, 0.f};

	/* 박혔을 때 정보 */
	CGameObject* m_pStuckTarget = nullptr;

public:
	static CArrow* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir, float ShotPower);

protected:
	virtual void Free() override;

};

