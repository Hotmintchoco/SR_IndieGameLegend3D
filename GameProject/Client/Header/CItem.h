#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CBoxCollider;
	class CTransform;
	class CTexture;
}

class CItem : public CGameObject
{
protected:
	explicit CItem(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CItem();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

	virtual void OnCollisionEnter(CGameObject* pOther) override;

	void CalculateAlphaZ();
	void BillBoard();

protected:
	virtual void Consume();

	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CBoxCollider* m_pColliderCom;
	Engine::CTexture* m_pTextureCom;

	_vec3 m_vSpawnPos = _vec3{0.f, 0.f, 0.f};

public:

protected:
	virtual void		Free();
};

