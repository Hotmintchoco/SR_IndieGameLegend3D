#pragma once

#include "CGameObject.h"
#include "Client_Enum.h"

namespace Engine
{
	class CTransform;
	class CCollider;
	class CTexture;
	class CRcTex;
	class CCamera;
}

class CPlayerAnimator;
class CPlayerPartTex;
class CPlayerMovement;
class CPlayer_MG1;
class CCamera_MG1;
class CBox_MG1;

class CPlayer_MG1 : public CGameObject
{
protected:
	explicit CPlayer_MG1(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer_MG1();

public:
	virtual HRESULT	Ready_GameObject() override;
	virtual _int Update_GameObject(_float fTimeDelta) override;
	virtual void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual void Render_GameObject() override;

	virtual	void OnCollisionEnter(COLLINFO eCollInfo) override;
	virtual	void OnCollisionStay(COLLINFO eCollInfo) override;

	/* HP 관련 */
	void OnHit(CGameObject* pSrcObj);
	void Revive();
	void OnDead();
	void RestoreHP(int iAmount);

	/* 입력 관련 */
	void SetInputEnabled(bool bFlag, float fFixedTime = -1.f);

	/* 가짜 스케일 */
	void SetPseudoScale(float fScale);

	inline CTransform* GetTransform() { return m_pTransformCom; }

	inline void SetCamera(CCamera_MG1* pCamera) { m_pCamera = pCamera; }

private:
	HRESULT	Add_Component();
	void Update_Input(const _float& fTimeDelta);
	void UpdateInput();

public:
	_float Get_Yaw() { return m_fYaw; }
	_float Get_Pitch() { return m_fPitch; }
	void Set_Camera1(CCamera* pCamera) { m_pCamera1 = pCamera; }
	void Set_Camera2(CCamera* pCamera) { m_pCamera2 = pCamera; }

	void Set_Box1(CBox_MG1* pBox) { m_pBox1 = pBox; }
	void Set_Box2(CBox_MG1* pBox) { m_pBox2 = pBox; }

	_bool Is_Connection() { return m_bConnection; }

	void Check_Connection();
private:
	_float m_fYaw = 0.f;
	_float m_fPitch = 0.f;
	_int m_iCameraType = 1;

	CCamera* m_pCamera1 = nullptr;
	CCamera* m_pCamera2 = nullptr;

	_bool m_bConnection = false;

	CBox_MG1* m_pBox1 = nullptr;
	CBox_MG1* m_pBox2 = nullptr;




	/* 카메라 뷰 */
	CCamera_MG1* m_pCamera = nullptr;
	//void OnCameraViewChanged(const CAMERA_MODE& Ctx);

	void SyncCameraYaw();
	float m_fInputYaw = 0.f;

	/* 기본 컴포넌트 */
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CCollider* m_pColliderCom = nullptr;
	Engine::CRcTex* m_pBufferCom2 = nullptr;

	/* 캐릭터 애니메이션 관련 */
	void OnActionAnimationFinished(const EPlayerActionState& Ctx);
	CPlayerAnimator* m_pAnimator = nullptr;
	CPlayerPartTex* m_pBufferCom[PP_END] = { nullptr };
	CTransform* m_pBufferTransformCom[PP_END] = { nullptr };
	CTransform* m_pVisualRootTransform = nullptr;
	bool m_bInputYawIgnored = false;
	CTransform* m_pAnimRootTransform = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;


	/* 캐릭터 작아짐 연출용 */
	float m_fPseudoScale = 1.f;
	float m_fColliderScale = 0.5f;

	/* 캐릭터 기본 스탯 */
	int m_iMaxHP = 12;
	int m_iHP = m_iMaxHP;

	/* 캐릭터 움직임 관련 */
	CPlayerMovement* m_pMovement = nullptr;

	/* 피격 관련 */
	bool m_bInvincible = false;
	float m_fInvincibleTime = 1.f;
	float m_fLeftInvincibleTime = 1.f;

	/* 입력 막기 */
	bool m_bInputEnabled = true;
	float m_fLeftInputDisabledTime = 0.f;


public:
	static CPlayer_MG1* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

