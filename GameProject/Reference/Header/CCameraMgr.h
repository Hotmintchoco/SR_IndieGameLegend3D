#pragma once

#include "CBase.h"
#include "CCamera.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CCameraMgr : public CBase
{
	DECLARE_SINGLETON(CCameraMgr)

private:
	explicit CCameraMgr();
	virtual ~CCameraMgr();

public:
	void		Update_Camera(const _float& fTimeDelta);
	void		LateUpdate_Camera(const _float& fTimeDelta);
	void		Key_Input();

public:
	HRESULT		Ready_Camera(const _tchar* pCameraTag, CAMERAID tagCameraType, LPDIRECT3DDEVICE9 pGraphicDev);
	HRESULT		Select_Camera(const _tchar* pCameraTag);
	HRESULT		Get_CamLook(_vec3* pLook);
	HRESULT		Get_CameraAngle(_float* pAngle);


private:
	CCamera*	Find_Camera(const _tchar* pCameraTag);

private:
	map<const _tchar*, CCamera*>			m_mapCamera;
	pair<const _tchar* , CCamera*>			m_pCurCamera; // 현재 선택한 카메라 정보
	
	
private:
	virtual void Free();

};

END