#pragma once

#include "CGameObject.h"
#include "Client_Enum.h"
#include "CEventDelegate.h"

namespace Engine
{
	class CTransform;
	class CBoxCollider;
	class CTexture;
	class CPlyTex;
}

class CLiminalFrame;

class CColorArea : public CGameObject
{
protected:
	explicit CColorArea(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CColorArea();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;
	virtual void OnCollisionStay(COLLINFO eCollInfo) override;
	virtual void OnCollisionExit(COLLINFO eCollInfo) override;

	void SetColor(EColorType eType);
	inline EColorType GetColor() { return m_eType; }

	CEventDelegate<EColorType> m_OnColorAreaSatisfied;

private:
	HRESULT Add_Component();

	void StartTimer();
	void StopTimer();

private:
	CTransform* m_pTransformCom = nullptr;
	CPlyTex* m_pBufferCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CBoxCollider* m_pColliderCom = nullptr;

	EColorType m_eType = EColorType::NONE;

	/* 타이머 관련 */
	float m_fRequiredTime = 3.f;
	float m_fTimeAfterStart = 0.f;
	bool m_bIsTimerActive = false;
	bool m_bSatisfied = false;

	CLiminalFrame* m_pFrame = nullptr;

	inline static const unordered_map<EColorType, wstring> s_mapColor = {
		{EColorType::RED, L"Red"},
		{EColorType::GREEN, L"Green"},
		{EColorType::BLUE, L"Blue"},
		{EColorType::YELLOW, L"Yellow"},
		{EColorType::WHITE, L"White"}
	};

public:
	static CColorArea* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};