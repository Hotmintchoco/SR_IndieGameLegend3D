#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CRapidGunUltimateScope : public CGameObject
{
protected:
	explicit CRapidGunUltimateScope(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CRapidGunUltimateScope();

public:
	virtual	HRESULT Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	void SetSize(int iX, int iY);
	void Begin();
	void End();

private:
	HRESULT Add_Component();
	void UpdateAnimation(float fTimeDelta);

	CRcTex* m_pBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture* m_pTexture = nullptr;

	bool m_bPlayingReverse = false;
	float m_fTimeAfterPlay = 0.f;
	float m_fFrameInterval = 0.05f;
	int m_iTextureIndex = 0;
	int m_iTextureFrameCount = 1;

public:
	static CRapidGunUltimateScope* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};

