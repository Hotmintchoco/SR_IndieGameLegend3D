#pragma once

#include "CBase.h"
#include "CCameraObj.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CCameraMgr : public CBase
{
	DECLARE_SINGLETON(CCameraMgr)

private:
	explicit CCameraMgr();
	virtual ~CCameraMgr();

public:
	void		Update_Camera(const _float& fTimeDelta, const _vec3& vTargetLook, const _vec3& vTargetPos, const _vec3& vTargetRight);
	void		LateUpdate_Camera(const _float& fTimeDelta);
	void		Key_Input();

public:
	HRESULT		Ready_Camera(const _tchar* pCameraTag, CAMERAID tagCameraType, LPDIRECT3DDEVICE9 pGraphicDev);
	HRESULT		Select_Camera(const _tchar* pCameraTag);
	HRESULT		Get_CamLook(_vec3* pLook);
	HRESULT		Get_CameraAngle(_float* pAngle);

	/* 성철 : (임시) 카메라의 이름으로 카메라 포인터를 가져오는 함수 */
	inline CCameraObj* GetCamera(const _tchar* pCameraTag) { return m_mapCamera.at(pCameraTag); }
	/* --- */

	void		SetCameraMove(const _vec3& vStartPos, const _vec3& vEndPos, const _vec3& vLook, _float fTime);
	void		SetCameraMoveAt(const _vec3& vStartPos, const _vec3& vEndPos, const _vec3& vAt, _float fTime);
	void		ClearCameraMove();
	_bool		IsCameraMoving() const { return m_iMoveIndex < m_vecCameraMove.size(); }

private:
	struct CAMERA_MOVE
	{
		_vec3	vStartPos;
		_vec3	vEndPos;
		_vec3	vTarget;
		_bool	bLookAt;
		_float	fTime;
	};

private:
	CCameraObj* Find_Camera(const _tchar* pCameraTag);
	void		Update_CameraMove(const _float& fTimeDelta);
	void		Apply_CameraMove(const CAMERA_MOVE& tMove, _float fRatio);
	void		Push_CameraMove(CAMERA_MOVE tMove);

private:
	map<const _tchar*, CCameraObj*>			m_mapCamera;
	pair<const _tchar*, CCameraObj*>			m_pCurCamera; 

	vector<CAMERA_MOVE>						m_vecCameraMove;
	_uint									m_iMoveIndex;
	_float									m_fMoveDuring;


private:
	virtual void Free();

};

END