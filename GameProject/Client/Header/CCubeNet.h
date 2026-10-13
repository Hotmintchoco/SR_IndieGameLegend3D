#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CTransform;
	class CBoxCollider;
	class CTexture;
	class CPlyTex;
}

class CCubeNet : public CGameObject
{
protected:
	explicit CCubeNet(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCubeNet();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	inline CTransform* GetTransform() { return m_pTransformCom; }

	inline void SetGrabbed(bool bFlag) { m_bIsGrabbed = bFlag; }

protected:
	HRESULT Add_Component();

	CTransform* m_pTransformCom = nullptr;
	CPlyTex* m_pBufferCom = nullptr;
	CTexture* m_pTextureCom = nullptr; /* 하위 클래스에서 지정 */
	CBoxCollider* m_pColliderCom = nullptr;
	CTransform* m_pOutlineTransform = nullptr;
	CTexture* m_pOutlineTexture = nullptr;

	bool m_bIsPlayerOnGrabRange = false;
	bool m_bIsGrabbed = false;

public:
	static CCubeNet* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};