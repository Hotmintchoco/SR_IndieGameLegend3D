#include "pch.h"
#include "CCursorPolicyMgr.h"
#include "CDInputMgr.h"

IMPLEMENT_SINGLETON(CCursorPolicyMgr);

CCursorPolicyMgr::CCursorPolicyMgr()
{
	ShowCursor(FALSE);
}

CCursorPolicyMgr::~CCursorPolicyMgr()
{
}

void CCursorPolicyMgr::Update()
{
	KeyInput();

	if (m_bCursorFixed)
	{
		FixCursorToWindowCenter();
	}
}

void CCursorPolicyMgr::KeyInput()
{
	if (CDInputMgr::GetInstance()->Key_Down(DIK_TAB))
	{
		m_bCursorFixed = !m_bCursorFixed;

		if (m_bCursorFixed)
		{
			while (ShowCursor(FALSE) >= 0) {}
		}
		else
		{
			while (ShowCursor(TRUE) < 0) {}
		}
	}
}

void CCursorPolicyMgr::FixCursorToWindowCenter()
{
	POINT ptMouseCenter{ WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &ptMouseCenter);
	SetCursorPos(ptMouseCenter.x, ptMouseCenter.y);
}

void CCursorPolicyMgr::Free()
{
}