#pragma once

#include "Engine_Define.h"
#include "CBase.h"

BEGIN(Engine)

class ENGINE_DLL CDInputMgr : public CBase
{
	DECLARE_SINGLETON(CDInputMgr)

private:
	explicit CDInputMgr(void);
	virtual ~CDInputMgr(void);

public:
	_byte	Get_DIKeyState(_byte byKeyID)
	{
		return m_byKeyState[byKeyID];
	}

	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse)
	{
		return m_tMouseState.rgbButtons[eMouse];
	}

	// 현재 마우스의 특정 축 좌표를 반환(X, Y, Z)
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
	{
		return *(((_long*)&m_tMouseState) + eMouseState);
	}

public:
	bool	Key_Press(_byte byKeyID);
	bool	Key_Down(_byte byKeyID);
	bool	Key_Up(_byte byKeyID);

	bool	Mouse_Press(MOUSEKEYSTATE eMouse);
	bool	Mouse_Down(MOUSEKEYSTATE eMouse);
	bool	Mouse_Up(MOUSEKEYSTATE eMouse);

public:
	HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);
	void	Update_InputDev(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;

private:
	_byte					m_byKeyState[256];		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	_byte					m_byPrevKeyState[256];	// 이전 프레임의 키보드 상태

	DIMOUSESTATE			m_tMouseState;
	DIMOUSESTATE			m_tPrevMouseState;

public:
	virtual void	Free(void);

};
END


