#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CTransform;
	class CBoxCollider;
	class CTexture;
}

class CFlatPlayerTex;
class CFlatPlayerMovement;

enum FLAT_FSM { FLAT_IDLE, FLAT_WALK, FLAT_JUMP, FLAT_END };

class CFlatPlayer : public CGameObject
{
protected:
	explicit CFlatPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFlatPlayer();

public:
	virtual HRESULT	Ready_GameObject() override;
	virtual _int Update_GameObject(_float fTimeDelta) override;
	virtual void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual void Render_GameObject() override;

	virtual CTransform* GetTransform() { return m_pTransform; }
	virtual CFlatPlayerMovement* GetMovement() { return m_pMovement; }

	inline void SetInputEnabled(bool bFlag) { m_bInputEnabled = bFlag; }

	/* 2D 플레이어 크기 : x = 가로 폭, y = 세로 높이(월드 z 방향). 피벗은 발밑 가운데 */
	inline const _vec2& GetFlatSize() const { return m_vFlatSize; }

private:
	HRESULT Add_Component();
	void UpdateInput();
	void SetFSM(FLAT_FSM e);
	void UpdateAnimationStatus(float fTimeDelta);

	CTransform* m_pTransform = nullptr;
	CBoxCollider* m_pBoxCollider = nullptr;
	CTexture* m_pTexture[FLAT_END] = { nullptr };
	CFlatPlayerTex* m_pBuffer = nullptr;
	CFlatPlayerMovement* m_pMovement = nullptr;
	
	/* 애니메이션 */
	FLAT_FSM m_eFSM = FLAT_IDLE;
	int m_iTextureFrameCount[FLAT_END] = { -1 };
	float m_fFrameInterval[FLAT_END] = { 0.5f, 0.125f, FLT_MAX };
	float m_fTimeAfterPlay = 0.f;
	int m_iCurrentTextureIndex = 0;

	/* 입력 */
	bool m_bInputEnabled = false;

	/* 크기 : 텍스처가 바뀌면 여기만 고치면 콜라이더와 방 경계 제한이 같이 따라감 */
	_vec2 m_vFlatSize = { 1.f, 1.f };

public:
	static CFlatPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

