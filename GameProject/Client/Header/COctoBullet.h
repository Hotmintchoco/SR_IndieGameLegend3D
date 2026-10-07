#pragma once

#include "CProjectile.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
	class CSphereCollider;
}

class COctoBullet : public CProjectile
{
protected:
	explicit COctoBullet(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir);
	virtual ~COctoBullet();

public:
	virtual	HRESULT Ready_GameObject();
	virtual	_int Update_GameObject(_float fTimeDelta);
	virtual	void LateUpdate_GameObject(_float fTimeDelta);
	virtual	void Render_GameObject();

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;

private:
	HRESULT	Add_Component();
	void Animation(const _float& fTimeDelta);

	Engine::CRcTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CSphereCollider* m_pColliderCom = nullptr;

	inline static TProjectileData s_tData = []()-> TProjectileData {
		TProjectileData t;
		return t;
		}();

	/* 애니메이션 */
	float m_fFrameInterval = 0.1f;
	float m_fSingleFrameAccTime = 0.f;
	int m_iCurrentTexureIdx = 0;
	int m_iTotalFrameCount = -1;

public:
	static COctoBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir);

protected:
	virtual void Free() override;

};

