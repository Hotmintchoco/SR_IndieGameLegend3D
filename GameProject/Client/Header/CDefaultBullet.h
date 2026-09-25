#pragma once

#include "CProjectile.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
	class CSphereCollider;
}

class CDefaultBullet : public CProjectile
{
protected:
	explicit CDefaultBullet(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir);
	virtual ~CDefaultBullet();

public:
	virtual	HRESULT Ready_GameObject();
	virtual	_int Update_GameObject(const _float& fTimeDelta);
	virtual	void LateUpdate_GameObject(const _float& fTimeDelta);
	virtual	void Render_GameObject();

	virtual void OnCollisionEnter(CGameObject* pObject) override;

private:
	HRESULT	Add_Component();
	void Animation(const _float& fTimeDelta);

	Engine::CRcTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CSphereCollider* m_pColliderCom = nullptr;

	/* 초기값 */
	_vec3 m_vStart{ 0.f, 0.f, 0.f };
	_vec3 m_vDir{ 0.f, 0.f, 0.f };

	/* 애니메이션 */
	float m_fFrameInterval = 0.1f;
	float m_fSingleFrameAccTime = 0.f;
	int m_iCurrentTexureIdx = 0;
	int m_iTotalFrameCount = -1;

public:
	static CDefaultBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir);

protected:
	virtual void Free() override;

};

