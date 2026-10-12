#pragma once

#include "CGameObject.h"
#include "CEventDelegate.h"
#include "IRayTestable.h"
#include "IAnamorphicTarget.h"

namespace Engine
{
	class CTransform;
	class CPlyTex;
	class CBoxCollider;
	class CTexture;
}

class CButton : public CGameObject, public IRayTestable, public IAnamorphicTarget
{
protected:
	explicit CButton(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CButton();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;

	CEventDelegate<void> m_OnButtonPressed;

	/* IRayTestable */
	virtual vector<pair<Engine::CVIBuffer*, Engine::CTransform*>> GetRayTestTargetInfo() override;
	
	virtual CVIBuffer* GetBuffer() override;
	virtual CTransform* GetTransform() override;

protected:
	HRESULT Add_Component();

	Engine::CPlyTex* m_pUpBuffer = nullptr;
	Engine::CPlyTex* m_pDownBuffer = nullptr;
	Engine::CTexture* m_pUpTexture = nullptr;
	Engine::CTexture* m_pDownTexture = nullptr;
	CTransform* m_pTransform = nullptr;
	CBoxCollider* m_pCollider = nullptr;

	bool m_bPressed = false;

public:
	static CButton* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free();
};

