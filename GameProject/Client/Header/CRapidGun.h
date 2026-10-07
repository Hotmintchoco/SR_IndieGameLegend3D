#pragma once

#include "CWeapon.h"

namespace Engine
{
	class CPlyTex;
	class CTexture;
}

class CMonster;

class CRapidGun : public CWeapon
{
protected:
	explicit CRapidGun(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CRapidGun();

public:
	virtual	HRESULT Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

private:
	HRESULT	Add_Component();
	void RenderEditorPanel();
	virtual TWeaponOutput SpecialAttack(EInputState ePri, EInputState eSec) override;
	virtual TWeaponOutput StartUltimateAttack(EInputState ePri, EInputState eSec) override;
	virtual TWeaponOutput UpdateUltimateAttack(EInputState ePri, EInputState eSec) override;
	virtual TWeaponOutput EndUltimateAttack(EInputState ePri, EInputState eSec) override;
	void UpdateUltimateAttackStatus(_float fTimeDelta);

	Engine::CPlyTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;

	/* 궁극기 스탯 */
	float m_fUltimateTime = 5.f;
	float m_fTimeAfterUltimate = 0.f;
	CMonster* m_pUltTarget = nullptr;
	float m_fAngleLimit = D3DXToRadian(30.f);
	float m_fUltimateAttackInterval = 0.075f;

public:
	static CRapidGun* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

