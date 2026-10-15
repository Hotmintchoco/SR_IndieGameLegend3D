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
	class CCalculator;
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

	inline CTransform* GetTransform() { return m_pTransformCom; }

private:
	HRESULT	Add_Component();
	void Update_Input(const _float& fTimeDelta);
	void UpdateInput();

	void Jump(const _float& fTimeDelta);

	_bool m_bJumpStart = false;
	_bool m_bJumpState = false;
	_bool m_bJumpState2 = false;
	_bool m_bLandingState = true;
	_vec3 m_vJumpDirection = { 0.f,3.f,0.f };
public:
	_float Get_Yaw() { return m_fYaw; }
	_float Get_Pitch() { return m_fPitch; }
	void Set_Camera1(CCamera* pCamera) { m_pCamera1 = pCamera; }
	void Set_Camera2(CCamera* pCamera) { m_pCamera2 = pCamera; }

	void Set_Box1(CBox_MG1* pBox) { m_pBox1 = pBox; }
	void Set_Box2(CBox_MG1* pBox) { m_pBox2 = pBox; }

	void Set_Box3(CBox_MG1* pBox) { m_pBox3 = pBox; }
	void Set_Box4(CBox_MG1* pBox) { m_pBox4 = pBox; }

	void Check_Connection();
	void Check_Connection2();

	_vec3 Get_OriginScale() { return m_vOriginScale; }
private:
	_float m_fYaw = 0.f;
	_float m_fPitch = 0.f;
	_int m_iCameraType = 1;

	CCamera* m_pCamera1 = nullptr;
	CCamera* m_pCamera2 = nullptr;

	_bool m_bConnection = false;
	_bool m_bConnectionStart = false;

	CBox_MG1* m_pBox1 = nullptr;
	CBox_MG1* m_pBox2 = nullptr;

	_vec3 m_vBoxOriginPos = { 0.f,0.f,0.f };
	_vec3 m_vBoxOriginRight = { 0.f,0.f,0.f };
	_vec3 m_vBoxOriginScale = { 0.f,0.f,0.f };

	_bool m_bConnection2 = false;
	_bool m_bConnectionStart2 = false;

	CBox_MG1* m_pBox3 = nullptr;
	CBox_MG1* m_pBox4 = nullptr;

	_vec3 m_vBoxOriginPos2 = { 0.f,0.f,0.f };
	_vec3 m_vBoxOriginRight2 = { 0.f,0.f,0.f };
	_vec3 m_vBoxOriginScale2 = { 0.f,0.f,0.f };


	_vec3 m_vOriginScale = { 0.f,0.f,0.f };


public :
	/* 기본 컴포넌트 */
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CCollider* m_pColliderCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CCalculator* m_pCalculatorCom = nullptr;
	//임시 추가
	Engine::CRcTex* m_pBufferCom2 = nullptr;


public:
	static CPlayer_MG1* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

