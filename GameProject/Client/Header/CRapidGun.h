#pragma once

#include "CWeapon.h"

namespace Engine
{
	class CPlyTex;
	class CTexture;
}

class CRapidGun : public CWeapon
{
protected:
	explicit CRapidGun(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CRapidGun();

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

public:
	static CRapidGun* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

