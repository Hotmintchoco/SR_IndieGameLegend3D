#pragma once

#include "CWeapon.h"

namespace Engine
{
	class CPlyTex;
	class CTexture;
}

class CShotGun : public CWeapon
{
protected:
	explicit CShotGun(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CShotGun();

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

	int m_iBulletPerSpecialAtk = 10;

	/* 샷건 총알 발사 노이즈 계산 시 필요 */
	_matrix m_matWorldCached;

public:
	static CShotGun* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

