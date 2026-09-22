#pragma once

#include "CTile.h"

namespace Engine
{
	class CPlyTex;
	class CTransform;
	class CTexture;
	class CBoxCollider;
}

class CButtonTile : public CTile
{
protected:
	explicit CButtonTile(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CButtonTile(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, bool bFixed);
	virtual ~CButtonTile();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	virtual void OnCollisionEnter(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther) override;

protected:
	HRESULT			Add_Component();

	Engine::CPlyTex* m_pUpBufferCom = nullptr;
	Engine::CPlyTex* m_pDownBufferCom = nullptr;
	Engine::CTexture* m_pUpTextureCom = nullptr;
	Engine::CTexture* m_pDownTextureCom = nullptr;
	Engine::CBoxCollider* m_pColliderCom = nullptr;

	bool m_bPressed = false; // 현재 눌림 상태
	bool m_bFixed = true; // 눌렀다 떼었을 때 돌아오는지
	int m_iCollisionCount = 0;

public:
	static CButtonTile* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, bool bFixed);

private:
	virtual void		Free();
};
