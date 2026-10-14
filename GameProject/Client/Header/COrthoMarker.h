#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CTransform;
	class CTexture;
	class CRcTex;
}

/* 플랭크를 들고 있을 때 "지금 내려놓으면 여기에 놓인다"를 보여주는 표시
   - 위치와 표시 여부는 COrthoRoomLayer가 정한다 */
class COrthoMarker : public CGameObject
{
protected:
	explicit COrthoMarker(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~COrthoMarker();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	inline CTransform* GetTransform() { return m_pTransform; }
	inline void SetVisible(bool bFlag) { m_bVisible = bFlag; }

private:
	HRESULT Add_Component();

	CRcTex* m_pBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture* m_pTexture = nullptr;

	bool m_bVisible = false;

public:
	static COrthoMarker* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free() override;
};
