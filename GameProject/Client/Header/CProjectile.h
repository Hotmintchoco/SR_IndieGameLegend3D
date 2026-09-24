#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
	class CTransform;
	class CSphereCollider;
}

class CProjectile : public CGameObject
{
protected:
	explicit CProjectile(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir);
	virtual ~CProjectile();

public:
	virtual	HRESULT Ready_GameObject();
	virtual	_int Update_GameObject(const _float& fTimeDelta);
	virtual	void LateUpdate_GameObject(const _float& fTimeDelta);
	virtual	void Render_GameObject();

	virtual _uint GetProjectileID() { return m_iID; }

	virtual void OnCollisionEnter(CGameObject* pObject) override;

private:
	HRESULT	Add_Component();
	void Animation(const _float& fTimeDelta);
	void CheckLifeTime(const _float& fTimeDelta);
	void BillBoard();

	Engine::CRcTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CSphereCollider* m_pColliderCom = nullptr;

	/* 초기값 */
	_vec3 m_vStart{0.f, 0.f, 0.f};
	_vec3 m_vDir{0.f, 0.f, 0.f};

	/* 기본 속성 */
	float m_fSpeed = 3.f;
	float m_fLifeTime = 3.f;
	float m_fLeftLifeTime = m_fLifeTime;

	/* 애니메이션 */
	float m_fFrameInterval = 0.1f;
	float m_fSingleFrameAccTime = 0.f;
	int m_iCurrentTexureIdx = 0;
	int m_iTotalFrameCount = -1;

	/* 이름 구분용 ID */
	static _uint g_iProjectileID;
	_uint m_iID = -1;

public:
	static CProjectile* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir);

protected:
	virtual void Free() override;
};

