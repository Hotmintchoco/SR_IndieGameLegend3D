#pragma once

#include "CGameObject.h"
#include "Client_Enum.h"

namespace Engine
{
	class CPlyTex;
	class CTransform;
	class CTexture;
}

struct TVeilDesc
{
	EColorTexture eColor;
	vector<float> vecRadius;
	vector<int> vecOpacity;
};

class CVeilSphere;

class CAtmosphereVeil : public CGameObject
{
protected:
	explicit CAtmosphereVeil(LPDIRECT3DDEVICE9 pGraphicDev, const TVeilDesc& tDesc);
	virtual ~CAtmosphereVeil();

public:
	virtual	HRESULT Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	inline CPlyTex* GetBuffer() { return m_pBufferCom; }
	inline CTexture* GetTexture() { return m_pTextureCom; }

	CTransform* GetTargetTransform();

	virtual void Set_IsActive(_bool bIsActive) override;

private:
	HRESULT	Add_Component();
	void SpawnSphere();

	Engine::CPlyTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;

	CGameObject* m_pTarget = nullptr;
	vector<CVeilSphere*> m_vecSphere;

	TVeilDesc m_tDesc{};

public:
	static CAtmosphereVeil* Create(LPDIRECT3DDEVICE9 pGraphicDev, const TVeilDesc& tDesc);

private:
	virtual void Free() override;
};
