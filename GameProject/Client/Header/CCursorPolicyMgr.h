#pragma once

#include "CBase.h"
#include "Engine_Define.h"

class CCursorPolicyMgr : public CBase
{
	DECLARE_SINGLETON(CCursorPolicyMgr);

private:
	explicit CCursorPolicyMgr();
	virtual ~CCursorPolicyMgr();

public:
	void Update();
    void Set_MenuMode(bool bMenuMode);

	inline bool IsCursorFixed() { return m_bCursorFixed; }

private:
	void KeyInput();
	void FixCursorToWindowCenter();

	bool m_bCursorFixed = true;
    bool m_bMenuMode = false;

private:
	virtual void Free() override;
};

