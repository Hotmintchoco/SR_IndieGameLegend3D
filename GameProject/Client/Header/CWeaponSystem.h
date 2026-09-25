#pragma once

#include "CGameObject.h"
#include "Client_Enum.h"

class CWeapon;

class CWeaponSystem : public CGameObject
{
protected:
	explicit CWeaponSystem(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CWeaponSystem();

public:
	virtual	HRESULT Ready_GameObject() override;
	virtual	_int Update_GameObject(const _float& fTimeDelta) override;
	virtual	void LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual	void Render_GameObject() override;

	/* 에너지 아이템 획득 */
	void GainEnergy();

private:	
	HRESULT AddWeapon(EObjectType eType, const wstring& wstrName);
	void GetKeyInput();
	void SwitchWeaponTo(int iIndex);

	vector<CWeapon*> m_vecWeapon;
	int m_iCurrentIndex = 0;

	/* 특수 공격 */
	bool m_bSpecialAttackSwitchOn = false;
	float m_fSpecialAtkGauge = 0.f;

	/* 궁극기 */
	float m_fUltimateAtkGauge = 0.f;
	float m_bIsUltimateAttackReady = false;

	inline CWeapon* GetCurrentWeapon() { return m_vecWeapon.at(m_iCurrentIndex); }

public:
	static CWeaponSystem* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

