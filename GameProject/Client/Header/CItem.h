#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CBoxCollider;
	class CTransform;
	class CTexture;
}

class CItem : public CGameObject
{
protected:
	explicit CItem(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CItem();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

	virtual void OnCollisionEnter(CGameObject* pOther) override;

	void BillBoard();
	void Drop(_float fTimeDelta);
	void Attract_To_Player(const _float& fTimeDelta);
protected:
	virtual void Consume();

	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CBoxCollider* m_pColliderCom;
	Engine::CTexture* m_pTextureCom;

	_vec3 m_vSpawnPos = _vec3{0.f, 0.f, 0.f};
	_float m_fFrame;

	_bool m_bDropFinish;
	_float m_fDropTime;
	_float m_fDropDuration;

	_bool m_bAttractStart;
	_float m_fAttractTime;
	_float m_fAttractDuration;

	_float m_fLifeTime;
	_float m_fLifeDuration;

	_bool m_bVisible;
	_bool m_bBlinkStart;
	_float m_fBlinkTime;
	_float m_fBlinkDuration;
	_float m_fBlinkDuration2;
public:

protected:
	virtual void		Free();
};

