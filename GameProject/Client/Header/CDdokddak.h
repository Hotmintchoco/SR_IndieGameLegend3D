#pragma once

#include "CGameObject.h"
#include "Client_Enum.h"

namespace Engine
{
	class CTransform;
	class CBoxCollider;
	class CRcTex;
	class CTexture;
}

class CDdokddak : public CGameObject
{
protected:
	explicit CDdokddak(LPDIRECT3DDEVICE9 pGraphicDev, EDirection eDir);
	virtual ~CDdokddak();

public:
	virtual	HRESULT	Ready_GameObject();
	virtual	_int Update_GameObject(const _float& fTimeDelta);
	virtual	void LateUpdate_GameObject(const _float& fTimeDelta);
	virtual	void Render_GameObject();

	virtual void OnCollisionEnter(CGameObject* pOther) override;

protected:
	HRESULT Add_Component();
	void BillBoard();

protected:
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CBoxCollider* m_pColliderCom = nullptr;
	Engine::CRcTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;

	EDirection m_eInitDir = EDirection::NONE;
	_vec3 m_vDir = _vec3{ 0.f, 0.f, 0.f };
	float m_fSpeed = 8.f;
	bool m_bSkipCurrentFrameCollision = false;

public:
	static CDdokddak* Create(LPDIRECT3DDEVICE9 pGraphicDev, EDirection eDir);

protected:
	virtual void Free();
};