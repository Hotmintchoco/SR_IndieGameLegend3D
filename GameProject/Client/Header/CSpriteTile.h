#pragma once

#include "CTile.h"
#include "Client_Enum.h"
#include "Client_Struct.h"

namespace Engine
{
	class CPlaneTex;
	class CTexture;
}

class CSpriteTile : public CTile
{
protected:
	explicit CSpriteTile(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSpriteTile(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, int iTextureIdx);
	explicit CSpriteTile(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, int iTextureIdx, bool bResistContamination);
	virtual ~CSpriteTile();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	void Contaminate(EContaminateType eType, float fDuration);

	inline void SetResistContamination(bool bFlag) { m_bResistContamination = bFlag; }
	inline bool GetResistContamination() { return m_bResistContamination; }
	inline EContaminateType GetContaminationType() { return m_eContaminationType; }

protected:
	HRESULT			Add_Component();
	void UpdateAnimationTile(const Engine::_float& fTimeDelta);

	Engine::CPlaneTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;

	/* 텍스쳐 관련 */
	int m_iTextureIdx = -1;

	/* 오염 타일 관련 */
	EContaminateType m_eContaminationType = EContaminateType::NONE;
	bool m_bResistContamination = false;
	Engine::CTexture* m_pAnimTextureCom = nullptr;
	bool m_bContaminated = false;
	float m_fContaminationLeftTime = 0.f;
	int m_iAnimTextureIndex = 0;
	float m_fAnimSingleFrameAccTime = 0.f;
	float m_fAnimFrameInterval = 0.2f;

public:
	static CSpriteTile* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, int iTextureIdx);
	static CSpriteTile* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, int iTextureIdx, bool bResistContamination);

protected:
	virtual void		Free();
};

