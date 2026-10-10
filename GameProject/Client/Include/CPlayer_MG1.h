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

	inline CTransform* GetTransform() { return m_pTransformCom; }

	//inline void SetCamera(CCamera_MG1* pCamera) { m_pCamera = pCamera; }

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
	//CCamera_MG1* m_pCamera = nullptr;

	/* 기본 컴포넌트 */
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CCollider* m_pColliderCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	//임시 추가
	Engine::CRcTex* m_pBufferCom2 = nullptr;




public:
	static CPlayer_MG1* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};

