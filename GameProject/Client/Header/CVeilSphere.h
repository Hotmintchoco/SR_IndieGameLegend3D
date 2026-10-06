#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CPlyTex;
	class CTexture;
	class CTransform;
}

class CAtmosphereVeil;

class CVeilSphere : public CGameObject
{
protected:
	explicit CVeilSphere(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CVeilSphere();

public:
	virtual	HRESULT Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	inline void SetOpacity(int iOpacity) { m_iOpacity = iOpacity; }
	void SetRadius(float fRadius);
	inline void SetParent(CAtmosphereVeil* pObj) { m_pParent = pObj; }

private:
	HRESULT	Add_Component();

	CTransform* m_pTransformCom = nullptr; /* 로컬 상태로 스케일 저장만 함 */

	CAtmosphereVeil* m_pParent = nullptr;
	int m_iOpacity = 100;

public:
	static CVeilSphere* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

