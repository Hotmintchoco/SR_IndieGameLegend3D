#pragma once
#include "CGameObject.h"
#include "Client_Enum.h"

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
	explicit CGameMachine(LPDIRECT3DDEVICE9 pGraphicDev, ESceneType eType);
	virtual ~CGameMachine();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	void Interact();

	void RegisterFunction(const function<void()>& F);

	
	void Set_Clear() { m_bClear = true; }
private:
	HRESULT			Add_Component();

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;

private:
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CSphereCollider* m_pColliderCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CPlyTex* m_pBufferCom = nullptr;

	ESceneType m_eSceneType = ESceneType::SCENE_NONE;

	function<void()> m_F = nullptr;

	_bool m_bClear = false;

public:
	static CGameMachine* Create(LPDIRECT3DDEVICE9 pGraphicDev, ESceneType eType);

private:
	virtual void		Free();
};

