#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

struct TLiminalUltimateTargetInfo
{
	_vec3 vWorldPos;
	float fDmgRatio;
};

struct TLiminalUltimateRenderInfo
{
	_vec3 vScreenPos;
	int iTextureIndex;
	bool bMaxCharged;
};

class CLiminalGunUltimateEffect : public CGameObject
{
protected:
	explicit CLiminalGunUltimateEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLiminalGunUltimateEffect();

public:
	virtual	HRESULT Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	void SetSize(int iX, int iY);
	void AddTargetInfo(const TLiminalUltimateTargetInfo& tInfo);

private:
	HRESULT Add_Component();
	_vec3 GetScreenPos(const _vec3& vWorldPos);
	void UpdateRenderInfo();

	CRcTex* m_pBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture* m_pFixedRingTexture = nullptr;
	CTexture* m_pRingTexture = nullptr;
	CTexture* m_pSkullTexture = nullptr;

	int m_iRingTextureIndex = 0;
	int m_iRingTextureFrameCount = 1;

	vector<TLiminalUltimateTargetInfo> m_vecTargetInfo;
	vector<TLiminalUltimateRenderInfo> m_vecRenderInfo;

public:
	static CLiminalGunUltimateEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};

