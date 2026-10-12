#pragma once

#include "CFrustum.h"
#include "IRayTestable.h"

namespace Engine
{
	class CPlyTex;
	class CTexture;
	class CGameObject;
}

/* 빙판길에서 상호작용함을 가정 */
class CPushableFrustum : public CFrustum, public IRayTestable
{
protected:
	explicit CPushableFrustum(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPushableFrustum();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	virtual void	OnCollisionEnter(COLLINFO eCollInfo) override;
	virtual void	OnCollisionStay(COLLINFO eCollInfo) override;

	/* IRayTestable */
	virtual vector<pair<Engine::CVIBuffer*, Engine::CTransform*>> GetRayTestTargetInfo() override;

private:
	HRESULT			Add_Component();
	virtual void Destroy() override;
	void StartAccelerate(const _vec3& vDir);

protected:
	Engine::CPlyTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;

	bool m_bOnMoving = false;
	float m_fMaxSpeed = 1.f;
	float m_fCurSpeed = 0.f;
	float m_fAccAmount = 2.f;
	_vec3 m_vMoveDir{ 0.f, 0.f, 0.f };

public:
	static CPushableFrustum* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

