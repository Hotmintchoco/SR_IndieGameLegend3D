#pragma once

#include "CWeapon.h"

namespace Engine
{
	class CTexture;
}

class CVoxelBuffer;

class CBow : public CWeapon
{
protected:
	explicit CBow(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBow();

public:
	virtual	HRESULT Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	virtual void ChargeStart() override;
	virtual void ChargeEnd() override;

private:
	HRESULT	Add_Component();
	void RenderEditorPanel();
	virtual void DefaultAttack() override;
	virtual void SpecialAttack() override;
	virtual void UltimateAttack() override;
	void ShootArrow();

	CVoxelBuffer* m_pBufferCom[4] = { nullptr };
	Engine::CTexture* m_pTextureCom[4] = { nullptr };

	float m_fChargeTime = 0.f;
	float m_fFullChargeTime = 0.6f;
	int m_iChargeLevel = 3;
	bool m_bOnCharging = false;
	int m_iRenderIdx = 0;

public:
	static CBow* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

