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
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;

	void BillBoard();
	void Drop(_float fTimeDelta);
	void Attract_To_Player(const _float& fTimeDelta);

public:
	void Set_SpawnPos(_vec3 vPos) { m_vSpawnPos = vPos; }

protected:
	virtual void Consume();

	Engine::CRcTex* m_pBufferCom = nullptr;
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CBoxCollider* m_pColliderCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;

	_vec3 m_vSpawnPos = _vec3{0.f, 0.f, 0.f};
	_float m_fFrame = 0.f;

	_bool m_bDropFinish = false;
	_float m_fDropTime = 0.f;
	_float m_fDropDuration = 0.25f;

	_bool m_bAttractStart = false;
	_float m_fAttractTime = 0.f;
	_float m_fAttractDuration = 0.25f;

	_float m_fLifeTime = 0.f;
	_float m_fLifeDuration = 10.f;

	_bool m_bVisible = true;
	_bool m_bBlinkStart = false;
	_float m_fBlinkTime = 0.f;
	_float m_fBlinkDuration = 0.25f;
	_float m_fBlinkDuration2 = 3.f;
public:

protected:
	virtual void		Free();
};

