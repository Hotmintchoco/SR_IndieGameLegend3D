#pragma once
#include "CGameObject.h"
#include "Client_Enum.h"

namespace Engine
{
	class CPlyTex;
	class CTransform;
	class CTexture;
	class CBoxCollider;
}

class CItemContainer : public CGameObject
{
protected:
	explicit CItemContainer(LPDIRECT3DDEVICE9 pGraphicDev, EObjectType eType);
	virtual ~CItemContainer();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();
	void Open();
	virtual void OnCollisionEnter(CGameObject* pOther) override;

private:
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CBoxCollider* m_pColliderCom = nullptr;
	Engine::CTexture* m_pPlateTexture = nullptr;
	Engine::CTexture* m_pSideTexture = nullptr;
	Engine::CPlyTex* m_pPlateBuffer = nullptr;
	Engine::CPlyTex* m_pSideBuffer = nullptr;

	bool m_bOnAnimation = false;
	float m_fSingleFrameAccTime = 0.f;
	float m_fFrameInterval = 0.07f;
	int m_iFrameCount = -1;
	int m_iCurrentFrame = 0;
	bool m_bAnimationFinished = false;

	EObjectType m_eInnerItemType = EObjectType::NONE;

public:
	static CItemContainer* Create(LPDIRECT3DDEVICE9 pGraphicDev, EObjectType eType);

private:
	virtual void		Free();
};

