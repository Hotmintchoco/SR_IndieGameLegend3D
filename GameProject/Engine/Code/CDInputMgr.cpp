#include "CDInputMgr.h"

IMPLEMENT_SINGLETON(CDInputMgr)

Engine::CDInputMgr::CDInputMgr(void)
{
	ZeroMemory(m_byKeyState, sizeof(m_byKeyState));
	ZeroMemory(m_byPrevKeyState, sizeof(m_byPrevKeyState));
}

Engine::CDInputMgr::~CDInputMgr(void)
{
	Free();
}

bool CDInputMgr::Key_Press(_byte byKeyID)
{
	return (m_byKeyState[byKeyID] & 0x80) != 0;
}

bool CDInputMgr::Key_Down(_byte byKeyID)
{
	return !(m_byPrevKeyState[byKeyID] & 0x80) && (m_byKeyState[byKeyID] & 0x80);
}

bool CDInputMgr::Key_Up(_byte byKeyID)
{
	return (m_byPrevKeyState[byKeyID] & 0x80) && !(m_byKeyState[byKeyID] & 0x80);
}

bool CDInputMgr::Mouse_Press(MOUSEKEYSTATE eMouse)
{
	return (m_tMouseState.rgbButtons[eMouse] & 0x80) != 0;
}

bool CDInputMgr::Mouse_Down(MOUSEKEYSTATE eMouse)
{
	return !(m_tPrevMouseState.rgbButtons[eMouse] & 0x80) && (m_tMouseState.rgbButtons[eMouse] & 0x80);
}

bool CDInputMgr::Mouse_Up(MOUSEKEYSTATE eMouse)
{
	return (m_tPrevMouseState.rgbButtons[eMouse] & 0x80) && !(m_tMouseState.rgbButtons[eMouse] & 0x80);
}

HRESULT Engine::CDInputMgr::Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{

	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		NULL)))
		return E_FAIL;

	// 키보드 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pKeyBoard->Acquire();


	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pMouse->Acquire();

	return S_OK;
}

void Engine::CDInputMgr::Update_InputDev(void)
{
	memcpy(m_byPrevKeyState, m_byKeyState, sizeof(m_byKeyState));
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);

	memcpy(&m_tPrevMouseState, &m_tMouseState, sizeof(DIMOUSESTATE));
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
}

void Engine::CDInputMgr::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}

