#pragma once

#include "Engine_Define.h"

class CRoomLayer;

class CDebugMgr
{
	DECLARE_SINGLETON(CDebugMgr);

private:
	explicit CDebugMgr();
	virtual ~CDebugMgr();

public:
	void Update(const float fTimeDelta);
	void Render();

	inline bool IsPanelOpened() { return m_bPanelOpen; }

private:
	bool m_bPanelOpen = false;

private:
	virtual void Free();
};

