#pragma once
#include "CCameraObj.h"
#include "Engine_Define.h"
#include <queue>

BEGIN(Engine)

class ENGINE_DLL CCameraWorking : public CCameraObj
{

private:
	explicit CCameraWorking(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCameraWorking();

public:
	HRESULT			Ready_Camera() override;
	void			Update_Camera(const _float& fTimeDelta, const _vec3& vTargetLook, const _vec3& vTargetPos, const _vec3& vTargetRight) override;
	void			LateUpdate_Camera(const _float& fTimeDelta) override;
	void			Input_Camera() override { }
	void			Reset_Setting() override { }
	void			SetCameraMove(const CAMERA_MOVE& camMoveInfo);
	void			ClearCameraMove();
	_bool			GetIsMoving() { return m_bIsMoving; }

private:
	
	HRESULT			SetEyePos(const CAMERA_MOVE& camMoveInfo);
	HRESULT			SetAtPos(const CAMERA_MOVE& camMoveInfo);

	queue<CAMERA_MOVE>		m_queCameraMove;
	_float					m_fMoveTimer;
	_bool					m_bIsMoving;

public:
	static CCameraObj* Create(LPDIRECT3DDEVICE9 pGraphicDev);


private:
	virtual void	Free();

};

END
