#pragma once

#include "CGameObject.h"
#include "IReflectable.h"
#include "Client_Enum.h"

struct TRoomEventCtx;

namespace Engine
{
	class CPlyTex;
	class CTransform;
	class CTexture;
	class CCollider;
	class CBoxCollider;
}

class CWall : public CGameObject, public IReflectable
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
	virtual			void		OnCollisionStay(CGameObject* pOther) override;

	inline EWallDir GetDir() { return m_eDir; };
	inline bool HasDoor() { return m_bHasDoor; };

	/* IReflectable */
	virtual const _vec3 GetNormal() override;

private:
	HRESULT Add_Component();
	void InitializeCollider();
	void OnRoomEvent(const TRoomEventCtx& t);
	void BlockDoor(bool bBlock);

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

