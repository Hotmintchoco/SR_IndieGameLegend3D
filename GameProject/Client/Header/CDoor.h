#pragma once

#include "CGameObject.h"
#include "IAnamorphicTarget.h"

struct TRoomEventCtx;

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CDoor : public CGameObject, public IAnamorphicTarget
{
protected:
	explicit CDoor(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDoor();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	inline int GetBindHandle() { return m_iBindHandle; }
	void Open();
	void Close();

	/* IAnamorphicTarget */
	virtual CVIBuffer* GetBuffer() override;
	virtual CTransform* GetTransform() override;

private:
	HRESULT			Add_Component();

private:
	void OnRoomEvent(const TRoomEventCtx& t);

	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	int m_iBindHandle = -1;

	int m_iTextureIdx = 4;
	bool m_bOnAnimation = false;
	const int m_iFrameCnt = 5;
	float m_fFrameInterval = 0.07f;
	float m_fSingleFrameAccTime = 0.f;
	int m_iPlayDirection = 1;

public:
	static CDoor* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

