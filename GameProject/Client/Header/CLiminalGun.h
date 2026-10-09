#pragma once

#include "CWeapon.h"

namespace Engine
{
	class CPlyTex;
	class CTexture;
}

class CLiminalObject;

class CLiminalGun : public CWeapon
{
protected:
	explicit CLiminalGun(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLiminalGun();

public:
	virtual	HRESULT Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

private:
	HRESULT	Add_Component();
	virtual TWeaponOutput SpecialAttack(EInputState ePri, EInputState eSec) override;
	virtual TWeaponOutput StartUltimateAttack(EInputState ePri, EInputState eSec) override;
	virtual TWeaponOutput UpdateUltimateAttack(EInputState ePri, EInputState eSec) override;
	virtual TWeaponOutput EndUltimateAttack(EInputState ePri, EInputState eSec) override;
	void RayCastToLiminalObject();
	void CaptureTransform(CLiminalObject* pObject);
	void CalculateView(CLiminalObject* pObject);
	void AdjustRotation(CLiminalObject* pObject);
	void UpdateUltimateAttackState(_float fTimeDelta);

	Engine::CPlyTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;

	CLiminalObject* m_pHolingObject = nullptr;
	_matrix m_matCapture; /* 캡쳐 시점의 카메라 기준 회전 변환을 저장하기 위함 */
	_vec3 m_vCaptureDisplacement{ 0.f, 0.f, 0.f }; /* 캡쳐 시점의 카메라로부터의 위치 변위를 저장하기 위함 */
	float m_fCaptureScale = 1.f;
	float m_fCaptureDist = 0.f;

	/* 궁극기 */
	float m_fDmgPerSecond = 3.f;
	float m_fTimeAfterUltimate = 0.f;
	float m_fDmgAccumulated = 0.f;
	float m_fMaxRadius = 30.f;
	float m_fMinRadius = 10.f;

public:
	static CLiminalGun* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

