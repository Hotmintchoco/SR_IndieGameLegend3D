#pragma once

#include "CWeapon.h"

namespace Engine
{
	class CPlyTex;
	class CTexture;
}

class CDefaultGun : public CWeapon
{
protected:
	explicit CDefaultGun(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDefaultGun();

public:
	virtual	HRESULT Ready_GameObject() override;
	virtual	_int Update_GameObject(const _float& fTimeDelta) override;
	virtual	void LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual	void Render_GameObject() override;

private:
	HRESULT	Add_Component();
	void RenderEditorPanel();
	virtual void SpecialAttack() override;
	virtual void UltimateAttack() override;

	Engine::CPlyTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;

	/* 특수 공격 */
	float m_fSpecialAtkInterval = 0.1f;

public:
	static CDefaultGun* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

