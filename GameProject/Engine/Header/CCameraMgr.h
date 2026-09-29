#pragma once

#include "CBase.h"
#include "CCameraObj.h"
#include "Engine_Define.h"
#include "CTransform.h"
#include "CCameraWorking.h"

BEGIN(Engine)

class ENGINE_DLL CCameraMgr : public CBase
{
	DECLARE_SINGLETON(CCameraMgr)

	explicit CCameraMgr();
private:
	virtual ~CCameraMgr();

public:
	void		Update_Camera(const _float& fTimeDelta, const _vec3& vTargetLook, const _vec3& vTargetPos, const _vec3& vTargetRight);
	void		LateUpdate_Camera(const _float& fTimeDelta);

public:
	HRESULT		Ready_Camera(const _tchar* pCameraTag, CAMERAID tagCameraType, LPDIRECT3DDEVICE9 pGraphicDev);
	HRESULT		Select_Camera(const _tchar* pCameraTag);
	HRESULT		Get_CamLook(_vec3* pLook);
	HRESULT		Get_CameraAngle(_float* pAngle);

	/* 성철 : (임시) 카메라의 이름으로 카메라 포인터를 가져오는 함수 */
	inline CCameraObj* GetCamera(const _tchar* pCameraTag) { return m_mapCamera.at(pCameraTag); }
	/* --- */

	HRESULT		SetCameraMove(const CAMERA_MOVE& camMoveInfo);
	HRESULT		ClearCameraMove();

private:
	CCameraObj* Find_Camera(const _tchar* pCameraTag);
	_vec3		RoomNormalizedToWorld(_int iRoomIndex, const _vec3& vNorm) const;

private:
	map<const _tchar*, CCameraObj*>			m_mapCamera;
	pair<const _tchar*, CCameraObj*>		m_pCurCamera;
	CCameraObj*								m_pCameraWorking;

	const _vec3								m_vCenterRoomPosition = _vec3{ 60.f, 0.f, 60.f };
	const _vec3								m_vInnerRoomSize = _vec3{ 13.f, 0.f, 11.f };
	const _vec3								m_vOuterRoomSize = _vec3{ 15.f, 0.f, 13.f };
	const _int								m_iRoomRowCount = 5;
	const _int								m_iRoomColCount = 5;
	const _float							m_fRoomHeight = 3.f;

private:
	virtual void Free();

};

END