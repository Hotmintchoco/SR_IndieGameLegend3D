#pragma once

#include	"CBase.h"
#include	"CScene.h"
#include "CRenderer.h"

BEGIN(Engine)

class ENGINE_DLL CManagement : public CBase
{
	DECLARE_SINGLETON(CManagement)

private:
	explicit	CManagement();
	virtual		~CManagement();

public:
	CComponent* Get_Component(COMPONENTID eID,
		const _tchar* pLayerTag,
		const _tchar* pObjTag,
		const _tchar* pComponentTag);

	CGameObject* Get_GameObject(const _tchar* pLayerTag, const _tchar* pObjTag);

	CLayer* Get_Layer(const _tchar* pLayerTag);

	inline CScene* GetCurrentScene() { return m_pScene; }

public:
	// 씬 전환 (이전 씬을 삭제하거나 유지할 수 있음)
	HRESULT			Change_Scene(_int iSceneIdx, CScene* pScene = nullptr, bool bDestoryOld = false);
	_int			Update_Scene(_float fTimeDelta);
	void			LateUpdate_Scene(_float fTimeDelta);
	void			Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	unordered_map<int, CScene*>	m_mapScene;
	CScene*						m_pScene;
	_int						m_iSceneIdx;

public:
	virtual void			Free();
};

END