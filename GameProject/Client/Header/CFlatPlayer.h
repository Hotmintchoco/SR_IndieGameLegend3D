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

	/* 끌 때는 마지막 이동 커맨드가 남아 계속 걸어가지 않도록 함께 멈춤 */
	void SetInputEnabled(bool bFlag);

	/* 2D 플레이어 크기 : x = 가로 폭, y = 세로 높이(월드 z 방향). 피벗은 발밑 가운데 */
	inline const _vec2& GetFlatSize() const { return m_vFlatSize; }

	/* 피격 표시 : 일정 시간 동안 렌더링을 껐다 켰다 반복 */
	void StartFlicker();

private:
	HRESULT Add_Component();
	void UpdateInput();
	void SetFSM(FLAT_FSM e);
	void UpdateAnimationStatus(float fTimeDelta);
	void UpdateFlicker(float fTimeDelta);

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
	/* 바라보는 방향 : 입력 부호로 갱신, 렌더 시 왼쪽이면 x 스케일 -1 (입력이 없으면 마지막 방향 유지) */
	bool m_bFacingRight = true;

	/* 피격 깜빡임 */
	bool m_bVisible = true;							/* 이번 프레임 렌더링 여부 */
	float m_fTimeAfterFlicker = FLT_MAX;				/* StartFlicker 이후 지난 시간 (FLT_MAX면 깜빡이지 않는 상태) */
	static constexpr float s_fFlickerDuration = 2.f;	/* 전체 깜빡임 시간 */
	static constexpr float s_fFlickerInterval = 0.1f;	/* 켜짐/꺼짐 한 번의 길이 */

	/* 크기 : 텍스처가 바뀌면 여기만 고치면 콜라이더와 방 경계 제한이 같이 따라감 */
	_vec2 m_vFlatSize = { 0.3f, 0.7f }; /* 쿼드(1x1) x 트랜스폼 스케일 0.7 */

public:
	static CFlatPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

