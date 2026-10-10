#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CPlyTex;
	class CTransform;
	class CTexture;
	class CBoxCollider;
}

class CJail : public CGameObject
{
protected:
	explicit CJail(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CJail();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;

private:
	HRESULT	Add_Component();
	void OpenDoor();

	CPlyTex* m_pDoorBufferCom = nullptr;
	CPlyTex* m_pPlateBufferCom = nullptr;
	CPlyTex* m_pBarBufferCom = nullptr;
	CTexture* m_pDoorTextureCom = nullptr;
	CTexture* m_pPlateTextureCom = nullptr;
	CTexture* m_pBarTextureCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CBoxCollider* m_pColliderCom = nullptr;

	int m_iDoorTextureIdx = 4;
	bool m_bOnAnimation = false;
	const int m_iFrameCnt = 5;
	float m_fFrameInterval = 0.07f;
	float m_fSingleFrameAccTime = 0.f;

public:
	static CJail* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};


