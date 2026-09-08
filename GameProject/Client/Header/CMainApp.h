#pragma once

#include "../../Base/CBase.h"
#include "CGraphicDev.h"
#include "CManagement.h"

class CMainApp : public CBase
{
private:
	explicit CMainApp();
	virtual ~CMainApp();

public:
	HRESULT		Ready_MainApp();
	int			Update_MainApp(const _float& fTimeDelta);
	void		LateUpdate_MainApp(const _float& fTimeDelta);
	void		Render_MainApp();

private:
	HRESULT		Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);
	HRESULT		Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	Engine::CGraphicDev* m_pDeviceClass;
	Engine::CManagement* m_pManagementClass;
	LPDIRECT3DDEVICE9	 m_pGraphicDev;

public:
	static CMainApp* Create();

private:
	virtual void	Free();

	

};

// 1. 평면의 방정식을 이용하여 지형을 타는 플레이어 상태를 만들어라.
// ax + by + cz + d = 0

// D3DXPLANE	Plane;
// D3DXPlaneFromPoints();