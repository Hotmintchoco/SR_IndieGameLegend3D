#pragma once

#include "CCubeNet.h"

class CCubeNetLava : public CCubeNet
{
protected:
	explicit CCubeNetLava(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCubeNetLava();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	virtual void OnCollisionStay(COLLINFO eCollInfo) override;

private:
	HRESULT Add_Component();

	/* 애니메이션 관련 */
	int m_iAnimFrameCount = -1;
	int m_iCurrentAnimFrame = 0;
	float m_fAnimFrameInterval = 0.1f;
	float m_fAnimAccTime = 0.f;

public:
	static CCubeNetLava* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};