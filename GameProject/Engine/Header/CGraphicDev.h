#pragma once

#include "Engine_Define.h"
#include "CBase.h"

BEGIN(Engine)

class ENGINE_DLL CGraphicDev :  public CBase
{
	DECLARE_SINGLETON(CGraphicDev)

private:
	explicit CGraphicDev();
	virtual~CGraphicDev();

public:
	LPDIRECT3DDEVICE9		Get_GraphicDev() { return m_pGraphicDev;  }

public:
	HRESULT		Ready_GraphicDev(HWND hWnd,
		WINMODE eMode, 
		const _uint& iSizeX,
		const _uint& iSizeY, 
		CGraphicDev** ppGraphicDev);

	void		Render_Begin(D3DXCOLOR Color);
	void		Render_End(HWND hWnd = nullptr);

private:
	// 1. 성능 조사를 하기 위한 컴객체 생성
	// 2. 그래픽 카드 성능 조사
	// 3. 그래픽 카드에서 제공되는 기능을 사용할 컴객체 생성

	LPDIRECT3D9			m_pSDK;			// 1
	LPDIRECT3DDEVICE9	m_pGraphicDev;	// 3
private:
	virtual void Free();
};

END



