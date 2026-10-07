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
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	inline CTransform* GetUltTransform() { return m_pUltRenderingTransform; }

private:
	HRESULT	Add_Component();
	void ShotSGBullet();
	virtual TWeaponOutput SpecialAttack(EInputState ePri, EInputState eSec) override;
	virtual TWeaponOutput UltimateAttack(EInputState ePri, EInputState eSec) override;
	void StartShootingSpree();
	void UpdateUltimateState(Engine::_float fTimeDelta);

	Engine::CPlyTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;

	int m_iBulletPerSpecialAtk = 10;

	/* 샷건 총알 발사 노이즈 계산 시 필요 */
	_matrix m_matWorldCached;

	/* 궁극기 스탯 */
	float m_fUltimateShotInterval = 0.07f;
	float m_fTimeAfterSingleShot = 0.0f;
	float m_fUltimateTime = 3.f;
	float m_fLeftUltimateTime = 0.f;
	bool m_bOnUltimateAttack = false;
	bool m_bRHandShotOrder = false;

	/* 궁극기 연출 시 메쉬 출력용 transform */
	CTransform* m_pUltRenderingTransform = nullptr;

public:
	static CShotGun* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

