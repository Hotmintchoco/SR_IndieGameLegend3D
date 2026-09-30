#pragma once

#include "CProjectile.h"

namespace Engine
{
	class CTexture;
	class CSphereCollider;
}

struct TArrowData : public TProjectileData
{
};

class CCrossBuffer;

class CArrow : public CProjectile
{
protected:
	explicit CArrow(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir, int iShotPower);
	virtual ~CArrow();

public:
	virtual	HRESULT Ready_GameObject();
	virtual	_int Update_GameObject(const _float& fTimeDelta);
	virtual	void LateUpdate_GameObject(const _float& fTimeDelta);
	virtual	void Render_GameObject();

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;

private:
	HRESULT	Add_Component();
	void LookTowardShotDirection();

	CCrossBuffer* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CSphereCollider* m_pColliderCom = nullptr;

	/* 초기값 */
	_vec3 m_vStart{ 0.f, 0.f, 0.f };
	_vec3 m_vDir{ 0.f, 0.f, 0.f };

	inline static TArrowData s_tData = []()-> TArrowData {
		TArrowData t;
		return t;
		}();

	int m_iShotPower = 0;
	float m_fSpeedPerShotPower = 5.f;

public:
	static CArrow* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir, int iShotPower);

protected:
	virtual void Free() override;

};

