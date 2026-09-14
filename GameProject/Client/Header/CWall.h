#pragma once

#include "CGameObject.h"
#include "Client_Enum.h"

namespace Engine
{
	class CPlyTex;
	class CTransform;
	class CTexture;
	class CCollider;
	class CBoxCollider;
}

class CWall : public CGameObject
{
protected:
	explicit CWall(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CWall(LPDIRECT3DDEVICE9 pGraphicDev, EWallDir eDir, bool bHasDoor);
	virtual ~CWall();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(CGameObject* pOther) override;

	inline EWallDir GetDir() { return m_eDir; };
	inline bool HasDoor() { return m_bHasDoor; };

private:
	HRESULT			Add_Component();
	void InitializeCollider();
	void OnRoomBegin();

private:
	Engine::CPlyTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCollider* m_pColliderCom[2];

	EWallDir m_eDir = EWallDir::NONE;
	bool m_bHasDoor = false;

public:
	static CWall* Create(LPDIRECT3DDEVICE9 pGraphicDev, EWallDir eDir, bool bHasDoor);

private:
	virtual void		Free();
};

