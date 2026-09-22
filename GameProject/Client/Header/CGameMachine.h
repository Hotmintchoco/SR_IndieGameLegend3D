#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CPlyTex;
	class CTransform;
	class CTexture;
	class CSphereCollider;
}

class CGameMachine : public CGameObject
{
protected:
	explicit CGameMachine(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGameMachine();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

	virtual void OnCollisionEnter(CGameObject* pOther) override;

private:
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CSphereCollider* m_pColliderCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CPlyTex* m_pBufferCom = nullptr;

public:
	static CGameMachine* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

