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

	/* 성철 */
	inline CCameraObj* GetCamera(const _tchar* pCameraTag) { return m_mapCamera.at(pCameraTag); }
	/* --- */

private:
	CCameraObj* Find_Camera(const _tchar* pCameraTag);

private:
	map<const _tchar*, CCameraObj*>			m_mapCamera;
	pair<const _tchar*, CCameraObj*>			m_pCurCamera; // 현재 선택한 카메라 정보


private:
	virtual void Free();

};

END