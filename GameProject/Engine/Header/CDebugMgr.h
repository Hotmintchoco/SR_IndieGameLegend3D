#pragma once

#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CDebugMgr
{
	DECLARE_SINGLETON(CDebugMgr);

private:
	explicit CDebugMgr();
	virtual ~CDebugMgr();

public:
	MESHRENDERMODE GetMeshMode() const { return m_eMeshMode; }
	void SetMeshMode(MESHRENDERMODE eMode) { m_eMeshMode = eMode; }

	bool GetShowCollider() const { return m_bShowCollider; }
	void SetShowCollider(bool bFlag) { m_bShowCollider = bFlag; }

private:
	bool m_bShowCollider = false;
	MESHRENDERMODE m_eMeshMode = MESHRENDERMODE::MESH_SOLID;

private:
	virtual void Free();
};

END