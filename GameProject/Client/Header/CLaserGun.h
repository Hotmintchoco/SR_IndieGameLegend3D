#pragma once
#include "CWeapon.h"
#include "CPostEffect.h"

namespace Engine
{
	class CPlyTex;
	class CTexture;
}

class CRibbon;
class CInvertEffect;

class CLaserGun : public CWeapon
{
protected:
	explicit CLaserGun(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLaserGun();

public:
	virtual	HRESULT Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	void OnExplosionPhaseEnded();

private:
	HRESULT	Add_Component();
	void RenderEditorPanel();
	void ShotLaser();
	virtual TWeaponOutput SpecialAttack(EInputState ePri, EInputState eSec) override;
	virtual TWeaponOutput StartUltimateAttack(EInputState ePri, EInputState eSec) override;
	virtual TWeaponOutput UpdateUltimateAttack(EInputState ePri, EInputState eSec) override;
	virtual TWeaponOutput EndUltimateAttack(EInputState ePri, EInputState eSec) override;

	Engine::CPlyTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;

	/* 궁극기 리본 캐싱용 */
	CRibbon* m_pRibbon = nullptr;
	/* 궁극기 셰이더 애니메이션 */
	float m_fTimeAfterEffect = 0.f;
	float m_fTotalEffectTransitionTime = 2.f;
	float m_fStartStrength = 0.f;
	float m_fEndStrength = 1.f;
	bool m_bOnEffectTransition = false;
	CInvertEffect* m_pShaderEffect = nullptr;
	POST_EFFECT m_eOldEffectType = POST_EFFECT::NONE;

	TBGMSnapshot m_tSavedBGM{};

public:
	static CLaserGun* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

