#pragma once

#include "CGameObject.h"
#include "Client_Enum.h"
#include "Client_Struct.h"

namespace Engine
{
	class CPlaneTex;
	class CTransform;
	class CTexture;
}

class CTile : public CGameObject
{
protected:
	explicit CTile(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTile(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, int iTextureIdx);
	explicit CTile(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, int iTextureIdx, bool bResistContamination);
	virtual ~CTile();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	void Contaminate(EContaminateType eType, float fDuration);
	
	inline int GetPositionIndex() { return m_iPositionIdx; }
	inline TTileIdx GetPositionIndex2D() { return m_tPositionIdx2D; }
	inline void SetResistContamination(bool bFlag) { m_bResistContamination = bFlag; }
	inline bool GetResistContamination() { return m_bResistContamination; }

private:
	HRESULT			Add_Component();

private:
	Engine::CPlaneTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	int m_iPositionIdx = -1;
	int m_iTextureIdx = -1;
	TTileIdx m_tPositionIdx2D = TTileIdx{ -1, -1 };

	/* 오염 타일 관련 */
	bool m_bResistContamination = false;
	Engine::CTexture* m_pAnimTextureCom = nullptr;
	bool m_bContaminated = false;
	float m_fContaminationLeftTime = 0.f;
	int m_iAnimTextureIndex = 0;
	float m_fAnimSingleFrameAccTime = 0.f;
	float m_fAnimFrameInterval = 0.2f;

public:
	static CTile* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, int iTextureIdx);
	static CTile* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, int iTextureIdx, bool bResistContamination);

private:
	virtual void		Free();
};

