#pragma once

#include "CGameObject.h"
#include "Client_Enum.h"
#include "Client_Struct.h"

class CWeapon;

class CWeaponSystem : public CGameObject
{
protected:
	explicit CWeaponSystem(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CWeaponSystem();

public:
	virtual	HRESULT Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	TWeaponSystemOutput UpdateInput(const TWeaponSystemInput& tInput);

	/* 에너지 아이템 획득 */
	void GainEnergy();

	HRESULT AddWeapon(EObjectType eType, const wstring& wstrName);

	inline void SetAnimationEnabled(bool bFlag) { m_bAnimationEnabled = bFlag; }
	inline CWeapon* GetCurrentWeapon() { return m_vecWeapon.at(m_iCurrentIndex); }

private:	
	void SwitchWeaponTo(int iIndex);

	vector<CWeapon*> m_vecWeapon;
	int m_iCurrentIndex = 0;

	/* 특수 공격 */
	bool m_bSpecialAttackSwitchOn = true;
	float m_fSpecialAtkGauge = 1.f;

	/* 궁극기 */
	float m_fUltimateAtkGauge = 1.f;
	float m_bIsUltimateAttackReady = true;

	/* 애니메이션 */
	bool m_bAnimationEnabled = true;

public:
	static CWeaponSystem* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

