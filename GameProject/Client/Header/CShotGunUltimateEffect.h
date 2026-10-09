#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CPlaneTex;
	class CPlyTex;
	class CTransform;
	class CTexture;
}

class CPlayer;

class CShotGunUltimateEffect : public CGameObject
{
protected:
	explicit CShotGunUltimateEffect(LPDIRECT3DDEVICE9 pGraphicDev, CPlayer* pTarget);
	virtual ~CShotGunUltimateEffect();

public:
	virtual	HRESULT Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

private:
	HRESULT Add_Component();
	void SyncTransformToTarget();
	void UpdateAnimation();

	CPlaneTex* m_pFloorBuffer = nullptr;
	CPlyTex* m_pWallBuffer = nullptr;
	CTransform* m_pFloorTransform = nullptr;
	CTransform* m_pWallTransform = nullptr;
	CTexture* m_pFloorTexture = nullptr;
	CTexture* m_pWallTexture = nullptr;

	float m_fTimeAfterBirth = 0.f;
	float m_fLifeTime = 3.0f;

	float m_fFloorFrameInterval = 0.07f;
	int m_iFloorIndex = 0;
	int m_iFloorFrameCount = 1;

	float m_fWallFrameInterval = 0.1f;
	int m_iWallIndex = 0;
	int m_iWallFrameCount = 1;

	CPlayer* m_pTarget = nullptr;

public:
	static CShotGunUltimateEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev, CPlayer* pTarget);

protected:
	virtual void Free() override;
};