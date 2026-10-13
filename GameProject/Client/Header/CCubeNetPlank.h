#pragma once

#include "CCubeNet.h"

namespace Engine
{
	class CTexture;
	class CTransform;
}

class CCubeNetPlank : public CCubeNet
{
protected:
	explicit CCubeNetPlank(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCubeNetPlank();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;
	virtual void OnCollisionStay(COLLINFO eCollInfo) override;
	virtual void OnCollisionExit(COLLINFO eCollInfo) override;

private:
	HRESULT Add_Component();

public:
	static CCubeNetPlank* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};
